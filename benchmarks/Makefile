#
# Copyright (c) 2015 Cossack Labs Limited
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#




scell-bearssl: scell.c
	gcc -I../src/ -I$(BEARSSL_PATH)/inc  -L../build_with_bearssl -L$(BEARSSL_PATH)/build -DCRYPTO_ENGINE_PATH=bearssl scell.c  -lthemis -lsoter -lbearssl -o scell-bearssl

scell-libsodium: scell.c
	gcc -I../src/ -I$(LIBSODIUM_PATH)/include  -L../build_with_libsodium -L$(LIBSODIUM_PATH)/.libs -DCRYPTO_ENGINE_PATH=libsodium scell.c  -lthemis -lsoter -lsodium -o scell-libsodium

scell-openssl: scell.c
	gcc -I../src/ -L../build scell.c -lthemis -lsoter -lcrypto -o scell-openssl

smessage-bearssl: smessage.c
	gcc -I../src/ -I$(BEARSSL_PATH)/inc  -L../build_with_bearssl -L$(BEARSSL_PATH)/build -DCRYPTO_ENGINE_PATH=bearssl smessage.c  -lthemis -lsoter -lbearssl -o smessage-bearssl

smessage-libsodium: smessage.c
	gcc -I../src/ -I$(LIBSODIUM_PATH)/include -L../build_with_libsodium -L$(LIBSODIUM_PATH)/.libs -DCRYPTO_ENGINE_PATH=libsodium smessage.c -lthemis -lsoter -lsodium -o smessage-libsodium

smessage-openssl: smessage.c
	gcc -I../src/ -L../build smessage.c -lthemis -lsoter -lcrypto -o smessage-openssl

ssession-bearssl: ssession.c
	gcc -I../src/ -I$(BEARSSL_PATH)/inc  -L../build_with_bearssl -L$(BEARSSL_PATH)/build -DCRYPTO_ENGINE_PATH=bearssl ssession.c  -lthemis -lsoter -lbearssl -o ssession-bearssl

ssession-libsodium: ssession.c
	gcc -I../src/ -I$(LIBSODIUM_PATH)/include -L../build_with_libsodium -L$(LIBSODIUM_PATH)/.libs -DCRYPTO_ENGINE_PATH=libsodium ssession.c -lthemis -lsoter -lsodium -o ssession-libsodium

ssession-openssl: ssession.c
	gcc -I../src/ -L../build ssession.c -lthemis -lsoter -lcrypto -o ssession-openssl

scell.csv: scell-bearssl scell-libsodium scell-openssl
	./benchmarks.sh scell 20000

smessage.csv: smessage-bearssl smessage-libsodium smessage-openssl
	./benchmarks.sh smessage 30000

ssession.csv: ssession-bearssl ssession-libsodium ssession-openssl
	./benchmarks.sh ssession 40000

png: scell.csv smessage.csv ssession.csv
	gnuplot scell.src >scell.png
	gnuplot smessage.src >smessage.png
	gnuplot ssession.src >ssession.png

clean:
	rm *.csv
	rm *.png
	rm scell-bearssl
	rm scell-libsodium
	rm scell-openssl
	rm smessage-bearssl
	rm smessage-libsodium
	rm smessage-openssl
	rm ssession-bearssl
	rm ssession-libsodium
	rm ssession-openssl