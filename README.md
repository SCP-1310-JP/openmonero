# Open Scp1310jp1 - a fully open sourced implementation of MyMonero backend

In this example [restbed](https://github.com/Corvusoft/restbed/) is used to
demonstrate how to provide Scp1310jp1 related JSON REST service. For this purpose,
 a service called Open Scp1310jp1 was developed.


Open Scp1310jp1 is an open source implementation of backend of
https://mymonero.com/. The frontend, which includes HTML, CSS, JavaScript, was adapted
from (and originally developed by) https://mymonero.com/.

Open Scp1310jp1's backend is open sourced, free
to use, host and modify. Additionally, some features were added/changed as compared
to MyMonero. They include:

 - google analytics, cloudflare, images and flash were removed.
 - frontend's html and javascript upgrades and cleanups.
 - transaction service fee was set to zero (same as in MyMonero).
 - the wallets generated use 25 word mnemonics, fully compatible with official scp1310jp1 wallets
(13 word mnemonics generated by MyMonero work as usual though).
 - import wallet fee was reduced.
 - added support of testnet and stagenet networks, and mainnet without relying transactions
 - improved handling of mempool, coinbase, locked and unlocked transactions.
 - view only mode added.
 - private tx key given to a user which can be used to prove sending sin to a recipient.
 - Cross-Origin Resource Sharing (CORS) disabled.
 - transaction confirmation window added to double check tx details before its commited.
 - ability to select transaction priority.
 - legacy address + payment_id system replaced with integrated addresses for better privacy.
 - free, time based imports of recent transactions added.
 - new transaction details window.
 - sending sin to a subaddress (not receiving nor generating subaddresses for now).

## Live stagenet version

- [http://172.104.45.209:81](http://172.104.45.209:81)

This is Open Scp1310jp1 running on stagnet network. You can use it to play around with it.
Please note that the live version isis running on cheap VPS, which may result in
performance issues.

## Current development version

All current changes, bug fixes and updates are done in the
[branch](https://github.com/moneroexamples/openscp1310jp1/tree/use_blocks_range).

## Screenshot

![Open Scp1310jp1](https://raw.githubusercontent.com/moneroexamples/openscp1310jp1/master/screenshot/screen1.png)


## Host it yourself

The Open Scp1310jp1 consists of four components that need to be setup for it to work:

 - MySql/Mariadb database - it stores user address (viewkey is not stored!),
 associated transactions, outputs, inputs and transaction import payments information.
 - Frontend - it is virtually same as that of MyMonero, except before mentioned differences.
  It consists of HTML, CSS, and JavaScript.
 - Scp1310jp1 daemon - daemon must be running and fully sync, as this is
 where all transaction data is fetched from and used. Daemon also commits txs
 from the Open Scp1310jp1 into the Scp1310jp1 network.
 - Backend - fully written in C++. It uses [restbed](https://github.com/Corvusoft/restbed/) to serve JSON REST to the frontend
 and [mysql++](http://www.tangentsoft.net/mysql++/) to interface the database. It also accesses Scp1310jp1 blockchain and "talks"
 with Scp1310jp1 deamon.


## Limitations

#### Performance

Open Scp1310jp1 is not as fast as MyMonero.
 This is because it is basic, easy to understand and
 straight forward implementation of the backend. Thus,
 it does not use any special memory buffers/caches for transactions,
 blocks, complex database structures and SQL queries. Also, no ongoing monitoring of user's
  transactions is happening, since user's viewkey is not stored (only its hash). Transaction search threads start when
  user logs in (viewkey and address are submitted to the search thread), and finish shorty
  after logout. Once the search threads stop, they can't be restarted without user logging
   in back, because viewkey is unknown.


## Example compilation on Ubuntu 18.04

Below are example and basic instructions on how to setup up and run Open Scp1310jp1 on Ubuntu 16.04.
For other Linux operating systems, the instructions are analogical.


#### Scp1310jp1 download and compilation

Download and compile recent Scp1310jp1 into your home folder:

```bash
# first install scp1310jp1 dependecines
sudo apt update

sudo apt install git build-essential cmake libboost-all-dev miniupnpc libunbound-dev graphviz doxygen libunwind-dev pkg-config libssl-dev libcurl4-openssl-dev libgtest-dev libreadline-dev libzmq3-dev libsodium-dev libpcsclite-dev

# go to home folder
cd ~

git clone --recursive https://github.com/scp1310jp1-project/scp1310jp1

cd scp1310jp1/

# checkout last scp1310jp1 version
git checkout -b last_release v0.12.0.0

make
```

#### Compilation of the Open Scp1310jp1 (don't run it yet)

Once Scp1310jp1 was downloaded and compiled, we can download Open Scp1310jp1 and compile it.
In fact we could postpone compilation to later, but
we can just do it now, to see if it compiles. But don't run it yet. It will not
work without database, setup frontend, and synced and running scp1310jp1 blockchain.

```bash
# need mysql++ libraries
sudo apt install libmysql++-dev

# go to home folder if still in ~/scp1310jp1
cd ~

git clone https://github.com/moneroexamples/openscp1310jp1.git

cd openscp1310jp1

mkdir build && cd build

cmake ..

# altearnatively can use cmake -DSCP1310JP1_DIR=/path/to/scp1310jp1_folder ..
# if scp1310jp1 is not in ~/scp1310jp1

make
```

#### Mysql/Mariadb

```bash
sudo apt install mysql-server
sudo mysql_secure_installation
```

Download `openscp1310jp1.sql` provided and setup the `openscp1310jp1` database. `openscp1310jp1.sql` script will
drop current `openscp1310jp1` if exist. So don't run it, if you have already some important information
in the `openscp1310jp1` database.

Assuming we are still in `build` folder:

```bash
# apply it to mysql
mysql -p -u root < ../sql/openscp1310jp1.sql
```

#### Lighttpd and frontend

```bash
sudo apt-get install lighttpd
```
Assuming you are still in `build` folder, copy frontend source files into lighttpd www folder.

```bash
sudo mkdir /var/www/html/openscp1310jp1
sudo cp -rvf ../html/* /var/www/html/openscp1310jp1/
```

Setup document root in `lighttpd.conf` into openscp1310jp1 folder

```bash
sudo vim /etc/lighttpd/lighttpd.conf
```

and change `server.document-root` into:

```bash
server.document-root    = "/var/www/html/openscp1310jp1"
```

Restart lighttpd to see the change:

```bash
sudo systemctl restart lighttpd
```

Go to localhost (http://127.0.0.1) and check if frontend is working.


#### Run Open Scp1310jp1

Command line options

```bash
./openscp1310jp1 -h
  -h [ --help ] [=arg(=1)] (=0)         produce help message
  -t [ --testnet ] [=arg(=1)] (=0)      use testnet blockchain
  -s [ --stagenet ] [=arg(=1)] (=0)     use stagenet blockchain
  --do-not-relay [=arg(=1)] (=0)        does not relay txs to other nodes.
                                        useful when testing construction and
                                        submiting txs
  -p [ --port ] arg (=1984)             default port for restbed service of
                                        Open Scp1310jp1
  -c [ --config-file ] arg (=./config/config.json)
                                        Config file path.
```

Other backend options are in `confing/config.json`.

Before running `openscp1310jp1`:

 - edit `config/config.js` file with your settings. Especially set `frontend-url` and `database`
 connection details.
 - set `apiUrl` in `html\js\config.js` and `nettype` option. Last slash `/` in `apiUrl` is important.
 If running backend for testnet or stagenet networks, frontend `nettype` must be set to  
 1 - TESTNET or 2 - STAGENET. 0 is for MAINNET.
 - make sure scp1310jp1 daemon is running and fully sync. If using testnet or stagenet networks, use scp1310jp1 daemon
 with `--testnet` or `--stagenet` flags!


To start for mainnet:
```bash
./openscp1310jp1
```

To start for testnet:
```bash
./openscp1310jp1 -t
```

To start for stagenet:
```bash
./openscp1310jp1 -s
```

To start for stagenet with non-default location of `config.json` file:

```bash
./openscp1310jp1 -s -c /path/to/config.json
```


## API calls

Example calls and their outputs are provided below.

#### get_version


```bash
curl  -w "\n" -X POST http://127.0.0.1:1984/get_version
```
Example output:

```json
{
  "api": 65536,
  "blockchain_height": 965507,
  "git_branch_name": "upgrade_angularjs",
  "last_git_commit_date": "2017-07-25",
  "last_git_commit_hash": "456f9d6",
  "scp1310jp1_version_full": "0.10.3.1-125f823"
}
```


`api` number is represented as `uint32_t`. In this case, `65536` represents
major version 1 and minor version 0.
In JavaScript, to get these numbers, one can do as follows:

```javascript
var api_major = response.data.api >> 16;
var api_minor = response.data.api & 0xffff;
```


### login

```bash
curl  -w "\n" -X POST http://127.0.0.1:1984/login -d '{"address": "A2VTvE8bC9APsWFn3mQzgW8Xfcy2SP2CRUArD6ZtthNaWDuuvyhtBcZ8WDuYMRt1HhcnNQvpXVUavEiZ9waTbyBhP6RM8TV", "view_key": "041a241325326f9d86519b714a9b7f78b29111551757eeb6334d39c21f8b7400"}'
```

```json
{"new_address":false,"status":"success"}
```


#### get_address_txs

```bash
curl  -w "\n" -X POST http://127.0.0.1:1984/get_address_txs -d '{"address": "A2VTvE8bC9APsWFn3mQzgW8Xfcy2SP2CRUArD6ZtthNaWDuuvyhtBcZ8WDuYMRt1HhcnNQvpXVUavEiZ9waTbyBhP6RM8TV", "view_key": "041a241325326f9d86519b714a9b7f78b29111551757eeb6334d39c21f8b7400"}'
```

Output (only part shown):

```json
{
  "blockchain_height": 965512,
  "new_address": false,
  "scanned_block_height": 961405,
  "scanned_block_timestamp": 1500969813,
  "scanned_height": 0,
  "start_height": 957190,
  "status": "success",
  "total_received": 32594830001895764,
  "total_received_unlocked": 32594830001895764,
  "transactions": [
    {
      "coinbase": false,
      "hash": "2877c449a7a9f0a507c7a6e4ae17b43d96dc44369092e57adc4e6d9ddcde1a68",
      "height": 812669,
      "id": 831631,
      "mempool": false,
      "mixin": 4,
      "payment_id": "",
      "spent_outputs": [
        {
          "amount": 13659082425875,
          "key_image": "0b6a04e1a1d7f149a8e8aeb91047b8ab4722de50554b88af4ed7646fd1929947",
          "mixin": 0,
          "out_index": 0,
          "tx_pub_key": ""
        }
      ],
      "timestamp": 1482567670,
      "total_received": 0,
      "total_sent": 13659082425875,
      "tx_pub_key": "41bd5cb51aa26fb58d41acd25711a7ecc2d19be0c24b296a9e362aebee61d4d0",
      "unlock_time": 0
    },
    {
      "coinbase": true,
      "hash": "1f76938b4deceb9e0722f02f4477006d3e96e2331552f726c47f297977434b9c",
      "height": 818908,
      "id": 838719,
      "mempool": false,
      "mixin": 0,
      "payment_id": "",
      "timestamp": 1483311688,
      "total_received": 13388479628538,
      "total_sent": 0,
      "tx_pub_key": "3c71217add3b7882e8370fe6b903bc48059a79580af5e095485afc88b3126d09",
      "unlock_time": 818968
    },
    {
      "coinbase": false,
      "hash": "53cb70ded276fbfcc68c98a8d9577b42c543bf1094d6cbb151fa05c9edb457be",
      "height": 818921,
      "id": 838735,
      "mempool": false,
      "mixin": 5,
      "payment_id": "",
      "spent_outputs": [
        {
          "amount": 13683584012406,
          "key_image": "437518836c315bf989c5cc28b935280345ed672d727122f6d6c5c5ff32e98224",
          "mixin": 0,
          "out_index": 0,
          "tx_pub_key": ""
        }
      ],
      "timestamp": 1483313063,
      "total_received": 12648774828503,
      "total_sent": 13683584012406,
      "tx_pub_key": "3eac7a5ce7dc0cc78172522cef4591a43b0e9aab643ac3b57554fd0dbc8ba86a",
      "unlock_time": 0
    }
  ]
}
```

#### get_address_info

```bash
curl  -w "\n" -X POST http://127.0.0.1:1984/get_address_info -d '{"address": "A2VTvE8bC9APsWFn3mQzgW8Xfcy2SP2CRUArD6ZtthNaWDuuvyhtBcZ8WDuYMRt1HhcnNQvpXVUavEiZ9waTbyBhP6RM8TV", "view_key": "041a241325326f9d86519b714a9b7f78b29111551757eeb6334d39c21f8b7400"}'
```

Output (only part shown):

```json
{
  "blockchain_height": 965513,
  "locked_funds": 0,
  "new_address": false,
  "scanned_block_height": 965513,
  "scanned_block_timestamp": 1501466493,
  "scanned_height": 0,
  "spent_outputs": [
    {
      "amount": 13683584012406,
      "key_image": "437518836c315bf989c5cc28b935280345ed672d727122f6d6c5c5ff32e98224",
      "mixin": 0,
      "out_index": 0,
      "tx_pub_key": ""
    },
    {
      "amount": 13683584012406,
      "key_image": "ac3088ce17cc608bcf86db65e9061fe4b9b02573b997944e4ebf7d8e64e4a3b4",
      "mixin": 0,
      "out_index": 0,
      "tx_pub_key": ""
    }
  ],
  "start_height": 855633,
  "total_received": 13481878608141995,
  "total_sent": 4699871131811773
}
```


#### get_tx

```bash
curl  -w "\n" -X POST http://127.0.0.1:1984/get_tx -d '{"tx_hash": "bfbfbb3bfa169731a092891795be1c3c923a018882ac0efc0ed3e79e2d2b2e54"}'
```

Output (only part shown):

```json
{
  "coinbase": false,
  "error": "",
  "fee": 22893920000,
  "mixin_no": 11,
  "no_confirmations": 2898,
  "pub_key": "b753c863c64565ae81630bfdbf497f06955b6ce041f656565809d04be6ef9343",
  "size": 13461,
  "status": "OK",
  "tx_hash": "bfbfbb3bfa169731a092891795be1c3c923a018882ac0efc0ed3e79e2d2b2e54",
  "tx_height": 960491,
  "sin_inputs": 0,
  "sin_outputs": 0
}
```

#### get_unspent_outs

```bash
curl  -w "\n" -X POST http://127.0.0.1:1984/get_unspent_outs -d '{"address": "A2VTvE8bC9APsWFn3mQzgW8Xfcy2SP2CRUArD6ZtthNaWDuuvyhtBcZ8WDuYMRt1HhcnNQvpXVUavEiZ9waTbyBhP6RM8TV", "view_key": "041a241325326f9d86519b714a9b7f78b29111551757eeb6334d39c21f8b7400","amount":"0","mixin":4,"use_dust":false,"dust_threshold":"1000000000"}'
```

Output (only part shown):

```json
{
  "amount": 2746682935584926,
  "outputs": [
    {
      "amount": 2450000000000,
      "global_index": 86136,
      "height": 839599,
      "index": 0,
      "public_key": "6f6a4023bfa407ca1ce37f7382d5ea7540a330575bd570094b5add5e8ded2dd9",
      "rct": "4aca9e9b9a5d63fcf409ac28191696cabb78c0ba14791152509ebe6db7f9311033a1dc75d69a1dad7523f65856d07487aad2bee2098f5566b6d92ec5a5c68f00653d241a9d7f16ff13df87825609e8b2353ec20e50d11f8133d234184d9f8b03",
      "spend_key_images": [
        "2818dae0940fb945185c562fcb0a496f3c3b551f33b7ddd7ec1b5ecd856166e6"
      ],
      "timestamp": "2017-02-01 05:53:28",
      "tx_hash": "9d17084091beedc55c8a0cd342e441b7c0d89eeca25ac151b4b91fe1e12051e7",
      "tx_id": 2117,
      "tx_prefix_hash": "849f11f6b012c1557f87692ca7a67bcb24a5a553078c4faed870b8982821feee",
      "tx_pub_key": "4219b1004fa64bde0213bf3c59b9e160af8603be03d79e13148c15fd598e3a0e"
    },
    {
      "amount": 10000000000,
      "global_index": 86550,
      "height": 840011,
      "index": 0,
      "public_key": "5506e8786b7634a77487f0938a00d3de6ab005e2f76ffee05fee68d5165382f1",
      "rct": "6f78720d6bb0287d78c50bfa41332232fbc4bc9b7f04e35681ff926cc156b1fbfb3b84a2dcdc8cb98b33be70b302e380944abc3e7a7cbf3e27614936eccca40203447622e149ba1e09a7062eb8910acf029f561ecdaa5a34e076baff5d63770e",
      "spend_key_images": [
        "9c82226bdf165fd2424d9a0ead661682bfab2fe644cd0bcef575ae16595c550c"
      ],
      "timestamp": "2017-02-02 03:18:18",
      "tx_hash": "8257367ca6def69dc3d280e8909c3cd01b230fc9922c9a89217db06f3fb41102",
      "tx_id": 2118,
      "tx_prefix_hash": "781a0ce699865987ec78ad331e8e89c0d14461f9881dd11a7a4541114b567568",
      "tx_pub_key": "29ceefd594b856c7c06d2423be17cd674b97d1d8f72907ace8d27f5b6aa9875c"
    }
  ],
  "per_kb_fee": 2480631964
}
```

#### get_random_outs

```bash
curl  -w "\n" -X POST http://127.0.0.1:1984/get_random_outs -d '{"amounts":["0","0"],"count":5}'
```

Output (only part shown):

```json
  "amount_outs": [
    {
      "amount": 0,
      "outputs": [
        {
          "global_index": 48449,
          "public_key": "637dbadf193fa9fd5c50c96af18f458a9b7d4844fdf7ffdfa3f62d51d6aff835",
          "rct": "0c908e1969edfe7824560104e44334b46ced17c9462eacb5aaa70e62ea34a394837c07d14bffc5a65e2dd14da395dd135bcc2e5ac70648782d76e4a9920cd007b9ffe319b796c555fb7713a270f21181a5ee5c8b01259becba0dd332b93a6c02"
        },
        {
          "global_index": 67558,
          "public_key": "6ca3a73512dadd669430f73809c949f3edf71728bea5201441c648c2d128c453",
          "rct": "999312ca1914895cf8a517c91a54a069d8fdc7205d7768173618e77fad2fde5c725604d666b101c9ae19c72e07cf5f821603a7b63efb5dfd8a7c0e36ed0c250fa92929cda49ddd0d34e664e15634ee59e958815764ec979b5ff0a72b3af6af0a"
        },
        {
          "global_index": 102186,
          "public_key": "c29d43f5d7c71a6f1b4f3286da3c296a083cf68728d85c268ee0c964a6c8c00e",
          "rct": "23aa82efdbd0c6878060496a13f7a707a6f45649b51de12d54d0cad14c5be5bd00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        },
        {
          "global_index": 96992,
          "public_key": "fb83d74a42abe65d5b8a6a906791202376b91e3459a31737ac62a401b7b9356c",
          "rct": "975b42f50cabb801091c90a4227bf7ba024ddacfeda7e5e0383f034b0bc8ba2500000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
        },
        {
          "global_index": 36848,
          "public_key": "2a8785e42a9446785cf189a40fca8f56a592dab9db1f38f8e8a3d2eb84b680ff",
          "rct": "a09e27b83917792bc8f6c51b40077aea329bf08b569e63657e2ac3529db5d0aa14f9f9fc45930577b43829119847ac857d69c00d12859a903cfcbf470819dc00b1bfa43aa979e46a0d0476cb36717e8d8374afe9af1fb3715091147cc9587e0e"
        }
      ]
    }
  ]
}
```

Need to add other.


## Scrap notes (just for myself)

### Generate your own ssl certificate

Setting up https and ssl certificates in restbed
 - https://github.com/Corvusoft/restbed/blob/34187502642144ab9f749ab40f5cdbd8cb17a54a/example/https_service/source/example.cpp

Based on the link above:

```bash
# Create Certificate
cd /tmp
openssl genrsa -out server.key 1024
openssl req -new -key server.key -out server.csr
openssl x509 -req -days 3650 -in server.csr -signkey server.key -out server.crt
openssl dhparam -out dh2048.pem 2048
```


## Other examples

Other examples can be found on  [github](https://github.com/moneroexamples?tab=repositories).
Please know that some of the examples/repositories are not
finished and may not work as intended.

## How can you help?

Constructive criticism, code and website edits are always good. They can be made through github.
