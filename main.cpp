#include "src/CmdLineOptions.h"
#include "src/MicroCore.h"
#include "src/YourScp1310jp1Requests.h"

#include <iostream>
#include <memory>
#include <cstdlib>


using namespace std;
using namespace restbed;

using boost::filesystem::path;

int
main(int ac, const char* av[])
{

// get command line options
sineg::CmdLineOptions opts {ac, av};

auto help_opt         = opts.get_option<bool>("help");

// if help was chosen, display help text and finish
if (*help_opt)
{
    return EXIT_SUCCESS;
}

auto do_not_relay_opt = opts.get_option<bool>("do-not-relay");
auto testnet_opt      = opts.get_option<bool>("testnet");
auto stagenet_opt     = opts.get_option<bool>("stagenet");
auto port_opt         = opts.get_option<string>("port");
auto config_file_opt  = opts.get_option<string>("config-file");


bool testnet          = *testnet_opt;
bool stagenet         = *stagenet_opt;
bool do_not_relay     = *do_not_relay_opt;

if (testnet && stagenet)
{
    cerr << "testnet and stagenet cannot be specified at the same time!" << endl;
    return EXIT_FAILURE;
}

// check if config-file provided exist
if (!boost::filesystem::exists(*config_file_opt))
{
    cerr << "Config file " << *config_file_opt
         << " does not exist" << endl;
    return EXIT_FAILURE;
}

nlohmann::json config_json;

try
{
    // try reading and parsing json config file provided
    std::ifstream i(*config_file_opt);
    i >> config_json;
}
catch (const std::exception& e)
{
    cerr << "Error reading confing file "
         << *config_file_opt << ": "
         << e.what() << endl;
    return EXIT_FAILURE;
}

//cast port number in string to uint16
uint16_t app_port   = boost::lexical_cast<uint16_t>(*port_opt);

// get the network type
cryptonote::network_type nettype = testnet ?
  cryptonote::network_type::TESTNET : stagenet ?
  cryptonote::network_type::STAGENET : cryptonote::network_type::MAINNET;

// set blockchain status monitoring thread parameters
sineg::CurrentBlockchainStatus::net_type
        = nettype;
sineg::CurrentBlockchainStatus::do_not_relay
        = do_not_relay;
sineg::CurrentBlockchainStatus::refresh_block_status_every_seconds
        = config_json["refresh_block_status_every_seconds"];
sineg::CurrentBlockchainStatus::max_number_of_blocks_to_import
        = config_json["max_number_of_blocks_to_import"];
sineg::CurrentBlockchainStatus::search_thread_life_in_seconds
        = config_json["search_thread_life_in_seconds"];
sineg::CurrentBlockchainStatus::import_fee
        = config_json["wallet_import"]["fee"];


string deamon_url;

// get blockchain path
// if confing.json paths are emtpy, defeault scp1310jp1
// paths are going to be used
path blockchain_path;

switch (nettype)
{
    case cryptonote::network_type::MAINNET:
        blockchain_path = path(config_json["blockchain-path"]["mainnet"].get<string>());
        deamon_url = config_json["daemon-url"]["mainnet"];
        sineg::CurrentBlockchainStatus::import_payment_address_str
                = config_json["wallet_import"]["mainnet"]["address"];
        sineg::CurrentBlockchainStatus::import_payment_viewkey_str
                = config_json["wallet_import"]["mainnet"]["viewkey"];
        break;
    case cryptonote::network_type::TESTNET:
        blockchain_path = path(config_json["blockchain-path"]["testnet"].get<string>());
        deamon_url = config_json["daemon-url"]["testnet"];
        sineg::CurrentBlockchainStatus::import_payment_address_str
                = config_json["wallet_import"]["testnet"]["address"];
        sineg::CurrentBlockchainStatus::import_payment_viewkey_str
                = config_json["wallet_import"]["testnet"]["viewkey"];
        break;
    case cryptonote::network_type::STAGENET:
        blockchain_path = path(config_json["blockchain-path"]["stagenet"].get<string>());
        deamon_url = config_json["daemon-url"]["stagenet"];
        sineg::CurrentBlockchainStatus::import_payment_address_str
                = config_json["wallet_import"]["stagenet"]["address"];
        sineg::CurrentBlockchainStatus::import_payment_viewkey_str
                = config_json["wallet_import"]["stagenet"]["viewkey"];
        break;
    default:
        cerr << "Invalid netowork type provided: " << static_cast<int>(nettype) << "\n";
        return EXIT_FAILURE;
}


if (!sineg::get_blockchain_path(blockchain_path, nettype))
{
    cerr << "Error getting blockchain path.\n";
    return EXIT_FAILURE;
}

// set remaining  blockchain status variables that depend on the network type
sineg::CurrentBlockchainStatus::blockchain_path
        =  blockchain_path.string();
sineg::CurrentBlockchainStatus::deamon_url
        = deamon_url;

cout << "Blockchain path: " << blockchain_path.string() << endl;


// set mysql/mariadb connection details
sineg::MySqlConnector::url      = config_json["database"]["url"];
sineg::MySqlConnector::port     = config_json["database"]["port"];
sineg::MySqlConnector::username = config_json["database"]["user"];
sineg::MySqlConnector::password = config_json["database"]["password"];
sineg::MySqlConnector::dbname   = config_json["database"]["dbname"];


// try connecting to the mysql
shared_ptr<sineg::MySqlAccounts> mysql_accounts;

try
{
    // MySqlAccounts will try connecting to the mysql database
    mysql_accounts = make_shared<sineg::MySqlAccounts>();
}
catch(std::exception const& e)
{
    cerr << e.what() << '\n';
    return EXIT_FAILURE;
}


// since CurrentBlockchainStatus class monitors current status
// of the blockchain (e.g., current height), its seems logical to
// make static objects for accessing the blockchain in this class.
// this way scp1310jp1 accessing blockchain variables (i.e. mcore and core_storage)
// are not passed around like crazy everywhere. Uri( "file:///tmp/dh2048.pem"
// There are here, and this is the only class that
// has direct access to blockchain and talks (using rpc calls)
// with the deamon.
if (!sineg::CurrentBlockchainStatus::init_scp1310jp1_blockchain())
{
    cerr << "Error accessing blockchain." << endl;
    return EXIT_FAILURE;
}

// launch the status monitoring thread so that it keeps track of blockchain
// info, e.g., current height. Information from this thread is used
// by tx searching threads that are launched for each user independently,
// when they log back or create new account.
sineg::CurrentBlockchainStatus::start_monitor_blockchain_thread();



// create REST JSON API services
sineg::YourScp1310jp1Requests open_scp1310jp1(mysql_accounts);

// create Open Scp1310jp1 APIs
MAKE_RESOURCE(login);
MAKE_RESOURCE(get_address_txs);
MAKE_RESOURCE(get_address_info);
MAKE_RESOURCE(get_unspent_outs);
MAKE_RESOURCE(get_random_outs);
MAKE_RESOURCE(submit_raw_tx);
MAKE_RESOURCE(import_wallet_request);
MAKE_RESOURCE(import_recent_wallet_request);
MAKE_RESOURCE(get_tx);
MAKE_RESOURCE(get_version);

// restbed service
Service service;

// Publish the Open Scp1310jp1 API created so that front end can use it
service.publish(login);
service.publish(get_address_txs);
service.publish(get_address_info);
service.publish(get_unspent_outs);
service.publish(get_random_outs);
service.publish(submit_raw_tx);
service.publish(import_wallet_request);
service.publish(import_recent_wallet_request);
service.publish(get_tx);
service.publish(get_version);

auto settings = make_shared<Settings>( );

if (config_json["ssl"]["enable"])
{
    // based on the example provided at
    // https://github.com/Corvusoft/restbed/blob/34187502642144ab9f749ab40f5cdbd8cb17a54a/example/https_service/source/example.cpp
    auto ssl_settings = make_shared<SSLSettings>( );

    Uri ssl_key = Uri(config_json["ssl"]["ssl-key"].get<string>());
    Uri ssl_crt = Uri(config_json["ssl"]["ssl-crt"].get<string>());
    Uri dh_pem  = Uri(config_json["ssl"]["dh-pem"].get<string>());

    ssl_settings->set_http_disabled(true);
    ssl_settings->set_port(app_port);
    ssl_settings->set_private_key(ssl_key);
    ssl_settings->set_certificate(ssl_crt);
    ssl_settings->set_temporary_diffie_hellman(dh_pem);

    settings->set_ssl_settings(ssl_settings);

    // can check using: curl -k -v -w'\n' -X POST 'https://127.0.0.1:1984/get_version'

    cout << "Start the service at https://127.0.0.1:" << app_port << endl;
}
else
{
    settings->set_port(app_port);
    settings->set_default_header( "Connection", "close" );

    cout << "Start the service at http://127.0.0.1:" << app_port  << endl;
}


service.start(settings);

return EXIT_SUCCESS;
}
