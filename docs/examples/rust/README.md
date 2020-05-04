# Rust examples

In this directory, we have some examples of Themis usage.

* [**keygen**](keygen.rs) — a tool for generating ECDSA keys (usable by other examples).
* [**secure_cell**](secure_cell.rs) — showcase of [Secure Cell](https://docs.cossacklabs.com/pages/secure-cell-cryptosystem/) API.
* [**secure_compare**](secure_compare.rs) — zero-knowledge secret comparison based on [Secure Comparator](https://docs.cossacklabs.com/pages/secure-comparator-cryptosystem/).
* <b>secure_message_*</b> — secure group chat implemented with [Secure Message](https://docs.cossacklabs.com/pages/secure-message-cryptosystem/)
  * [**secure_message_server**](secure_message_server.rs) — simple relay server.
  * [**secure_message_client_encrypt**](secure_message_client_encrypt.rs) — chat client which encrypts messages.
  * [**secure_message_client_verify**](secure_message_client_verify.rs) — chat client which signs and verifies messages.
* <b>secure_session_echo_*</b> — an example of secure network communication with [Secure Session](https://docs.cossacklabs.com/pages/secure-session-cryptosystem/).
  * [**secure_session_echo_client**](secure_session_echo_client.rs) — simple echo client using buffer-oriented API.
  * [**secure_session_echo_server**](secure_session_echo_server.rs) — simple echo server using callback API.

This is how you can run the examples using Cargo:

```
$ cargo run --example keygen -- --help
keygen 0.0.3
Generating ECDSA key pairs.

USAGE:
    keygen [OPTIONS]

FLAGS:
    -h, --help       Prints help information
    -V, --version    Prints version information

OPTIONS:
        --private <path>    Private key file (default: private.key)
        --public <path>     Public key file (default: public.key)
```

Note that the arguments are passed after `--`.


## keygen

This tool can be used to generate key files usable for other examples.


## secure_cell

This is a simple API demo, it has no command-line arguments.
Note how encrypted message length depends on the mode being used.

## secure_compare

This tool can be used to compare secrets over network without actually sharing them.
It is made possible by _Secure Comparator_.

The tool includes both the server and the client selectable via command-line.
Both accept the secrets on the _standard input_ and start comparison after input is complete (use Ctrl-D in a typical terminal for this).

A typical comparison session looks like this:

```console
$ echo "secret" | cargo run --example secure_compare -- server
[+] match OK
```

```console
$ cargo run --example secure_compare -- client
secret
^D
[+] match OK
```


## secure_message

This is a more complicated example of relay chat over UDP using _Secure Message_.
It is kept simple on purpose, but the same principle can be applied to properly framed TCP transports
as well as to using Tokio for async IO instead of blocking stdlib.

Usually you don’t need to specify custom options as the command-line defaults are expected to work right away.
But you can override the defaults for port assignment and key file locations if necessary.

First, you’ll need to generate the keys for clients.
It also may be useful to enable logging before starting the server.
This example uses [`env_logger` crate][env_logger] for logging
which is configurable via environment variable `RUST_LOG`.

[env_logger]: https://crates.io/crates/env_logger

```
$ export RUST_LOG=secure_message=info
$ cargo run --example keygen
```

Then you can start the server and some clients
(in separate terminal sessions):

```
$ cargo run --example secure_message_server
 INFO 2018-09-30T19:39:49Z: secure_message_server: listening on port 7573
 INFO 2018-09-30T19:40:33Z: secure_message_server: new peer: [::1]:56375
 INFO 2018-09-30T19:40:36Z: secure_message_server: new peer: [::1]:56376
```

```
$ cargo run --example secure_message_client_encrypt
2: hello
1: hello
```

The first message from the client will introduce it to the server, then the server will relay other clients’ messages to the newly-joined peer.

> Note: At the moment, you have to manually type in the nicknames.

The clients use the generated keys to secure communications.
You can observe the exchange with `tcpdump`:

```
$ sudo tcpdump -i any -n -X udp port 7573
tcpdump: data link type PKTAP
tcpdump: verbose output suppressed, use -v or -vv for full protocol decode
listening on any, link-type PKTAP (Apple DLT_PKTAP), capture size 262144 bytes
22:45:11.587870 IP6 ::1.56375 > ::1.7573: UDP, length 63
	0x0000:  1e00 0000 6004 da0e 0047 1140 0000 0000  ....`....G.@....
	0x0010:  0000 0000 0000 0000 0000 0001 0000 0000  ................
	0x0020:  0000 0000 0000 0000 0000 0001 dc37 1d95  .............7..
	0x0030:  0047 005a 2027 0426 3f00 0000 0001 0140  .G.Z.'.&?......@
	0x0040:  0c00 0000 1000 0000 0b00 0000 9786 7d70  ..............}p
	0x0050:  080f 1812 8aeb 0a92 18ca 91fb 008e 355c  ..............5\
	0x0060:  8e6a 657b 05f1 a365 3e40 a921 50cd 9a8c  .je{...e>@.!P...
	0x0070:  6825 9a                                  h%.
```

Some notable things about this example:

* The relay server has _zero knowledge_ of the encrypted message content.
* The sign/verify client does not encrypt the messages
  (as you may see with `tcpdump`). But it still verifies their integrity.

Currently all clients are expected to use the same keys.


## secure_session_echo

The server expects connections from clients and echoes back any messages sent by individual clients.
Communication between parties is secured using Secure Session.

Usually you don’t need to specify custom options as the command-line defaults are expected to work right away.
But you can override the port assignment if the default port is already in use in your system.
