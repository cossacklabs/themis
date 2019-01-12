// Copyright 2018 (c) rust-themis developers
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

extern crate byteorder;
#[macro_use]
extern crate clap;
extern crate themis;

use std::io::{self, Read, Write};
use std::net::{TcpListener, TcpStream};

use byteorder::{LittleEndian, ReadBytesExt, WriteBytesExt};
use themis::secure_comparator::SecureComparator;

fn main() {
    let matches = clap_app!(secure_compare =>
        (version: env!("CARGO_PKG_VERSION"))
        (about: "Zero-knowledge secret comparison with Secure Comparator.")
        (after_help:
            "Both the client and the server expect to read the secret from standard input\n\
             until the end before they proceed with comparison. Usually this is achieved\n\
             by piping a file into them. You can use interactive terminal as well, just\n\
             make sure to terminate the stream with ^D.")
        (@subcommand server =>
            (about: "Expect a client for comparison")
            (@arg port: -p --port [number] "Listening port (default: 7575)")
        )
        (@subcommand client =>
            (about: "Connect to server for comparison")
            (@arg address: -c --connect [address] "Server address (default: [::1]:7575)")
        )
    ).get_matches();

    let mut comparison = SecureComparator::new();

    let mut buffer = [0; 4096];
    loop {
        let bytes = io::stdin().read(&mut buffer).expect("read secret");
        if bytes == 0 {
            break;
        }
        comparison
            .append_secret(&buffer[..bytes])
            .expect("append secret");
    }

    if let Some(matches) = matches.subcommand_matches("client") {
        let address = matches.value_of("address").unwrap_or("[::1]:7575");
        let mut server = TcpStream::connect(address).expect("client connect");

        let mut request = comparison.begin_compare().expect("begin");

        while !comparison.is_complete() {
            send_msg(&request, &mut server).expect("send");
            let reply = receive_msg(&mut server).expect("receive");
            request = comparison.proceed_compare(&reply).expect("proceed");
        }
    }

    if let Some(matches) = matches.subcommand_matches("server") {
        let port = matches
            .value_of("port")
            .unwrap_or("7575")
            .parse()
            .expect("port number");
        let (mut client, _) = TcpListener::bind(("::", port))
            .expect("server bind")
            .accept()
            .expect("client accept");

        while !comparison.is_complete() {
            let request = receive_msg(&mut client).expect("receive");
            let reply = comparison.proceed_compare(&request).expect("proceed");
            send_msg(&reply, &mut client).expect("send");
        }
    }

    if comparison.get_result().expect("result") {
        println!("[+] match OK");
    } else {
        println!("[-] no match");
    }
}

// TCP is a streaming protocol so we'll need to frame our messages. We use a simple approach:
// prefix each message with its length in bytes serialized as a 4-byte value in little endian.

fn send_msg(message: &[u8], peer: &mut TcpStream) -> io::Result<()> {
    if message.len() > u32::max_value() as usize {
        return Err(io::Error::new(
            io::ErrorKind::InvalidInput,
            "too big message",
        ));
    }
    peer.write_u32::<LittleEndian>(message.len() as u32)?;
    peer.write_all(&message)
}

fn receive_msg(peer: &mut TcpStream) -> io::Result<Vec<u8>> {
    let len = peer.read_u32::<LittleEndian>()?;
    let mut msg = vec![0; len as usize];
    peer.read_exact(&mut msg)?;
    Ok(msg)
}
