//
// Created by mwo on 8/12/16.
//

#ifndef RESTBED_SIN_YOURSCP1310JP1REQUESTS_H
#define RESTBED_SIN_YOURSCP1310JP1REQUESTS_H

#include <iostream>
#include <functional>


#include "version.h"

#include "CurrentBlockchainStatus.h"
#include "MySqlAccounts.h"
#include "../gen/version.h"

#include "../ext/restbed/source/restbed"

#ifndef MAKE_RESOURCE
#define MAKE_RESOURCE(name) auto name = open_scp1310jp1.make_resource( \
                           &sineg::YourScp1310jp1Requests::name, "/" + string(#name));
#endif



// When making *any* change here, bump minor
// If the change is incompatible, then bump major and set minor to 0
// This ensures that RPC_VERSION always increases, that every change
// has its own version, and that clients can just test major to see
// whether they can talk to a given backend without having to know in
// advance which version they will stop working with
// Don't go over 32767 for any of these
#define OPENSCP1310JP1_RPC_VERSION_MAJOR 1
#define OPENSCP1310JP1_RPC_VERSION_MINOR 3
#define MAKE_OPENSCP1310JP1_RPC_VERSION(major,minor) (((major)<<16)|(minor))
#define OPENSCP1310JP1_RPC_VERSION \
    MAKE_OPENSCP1310JP1_RPC_VERSION(OPENSCP1310JP1_RPC_VERSION_MAJOR, OPENSCP1310JP1_RPC_VERSION_MINOR)


namespace sineg
{

using namespace std;
using namespace restbed;
using namespace nlohmann;


struct handel_
{
    using fetch_func_t = function< void ( const shared_ptr< Session >, const Bytes& ) >;

    fetch_func_t request_callback;

    handel_(const fetch_func_t& callback);

    void operator()(const shared_ptr< Session > session);
};


class YourScp1310jp1Requests
{

    // this manages all mysql queries
   shared_ptr<MySqlAccounts> sin_accounts;


public:

    YourScp1310jp1Requests(shared_ptr<MySqlAccounts> _acc);

    /**
     * A login request handler.
     *
     * It takes address and viewkey from the request
     * and check mysql if address/account exist. If yes,
     * it returns this account. If not, it creates new one.
     *
     * Once this complites, a thread is started that looks
     * for txs belonging to that account.
     *
     * @param session a Restbed session
     * @param body a POST body, i.e., json string
     */
    void
    login(const shared_ptr<Session> session, const Bytes & body);

    void
    get_address_txs(const shared_ptr< Session > session, const Bytes & body);

    void
    get_address_info(const shared_ptr< Session > session, const Bytes & body);

    void
    get_unspent_outs(const shared_ptr< Session > session, const Bytes & body);

    void
    get_random_outs(const shared_ptr< Session > session, const Bytes & body);

    void
    submit_raw_tx(const shared_ptr< Session > session, const Bytes & body);

    void
    import_wallet_request(const shared_ptr< Session > session, const Bytes & body);

    void
    import_recent_wallet_request(const shared_ptr< Session > session, const Bytes & body);

    void
    get_tx(const shared_ptr< Session > session, const Bytes & body);

    void
    get_version(const shared_ptr< Session > session, const Bytes & body);

    shared_ptr<Resource>
    make_resource(function< void (YourScp1310jp1Requests&, const shared_ptr< Session >, const Bytes& ) > handle_func,
                  const string& path);

    static void
    generic_options_handler( const shared_ptr< Session > session );

    static multimap<string, string>
    make_headers(
            const multimap<string, string>& extra_headers
            = multimap<string, string>());

    static void
    print_json_log(const string& text, const json& j);

    static inline string
    body_to_string(const Bytes & body);

    static inline json
    body_to_json(const Bytes & body);

    inline uint64_t
    get_current_blockchain_height();

private:

    bool
    login_and_start_search_thread(
            const string& sin_address,
            const string& viewkey,
            SinAccount& acc,
            json& j_response);


    inline void
    session_close(const shared_ptr< Session > session, string response_body);

    bool
    parse_request(const Bytes& body,
                  vector<string>& values_map,
                  json& j_request,
                  json& j_response);

};



}
#endif //RESTBED_SIN_YOURSCP1310JP1REQUESTS_H
