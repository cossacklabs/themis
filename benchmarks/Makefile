

scell-libsodium: scell.c
	gcc -I../src/ -I$(LIBSODIUM_PATH)/include  -L../build_with_libsodium -L$(LIBSODIUM_PATH)/.libs -DCRYPTO_ENGINE_PATH=libsodium scell.c  -lthemis -lsoter -lsodium -o scell-libsodium

scell-openssl: scell.c
	gcc -I../src/ -L../build scell.c -lthemis -lsoter -lcrypto -o scell-openssl

smessage-libsodium: smessage.c
	gcc -I../src/ -I$(LIBSODIUM_PATH)/include -L../build_with_libsodium -L$(LIBSODIUM_PATH)/.libs -DCRYPTO_ENGINE_PATH=libsodium smessage.c -lthemis -lsoter -lsodium -o smessage-libsodium

smessage-openssl: smessage.c
	gcc -I../src/ -L../build smessage.c -lthemis -lsoter -lcrypto -o smessage-openssl

ssession-libsodium: ssession.c
	gcc -I../src/ -I$(LIBSODIUM_PATH)/include -L../build_with_libsodium -L$(LIBSODIUM_PATH)/.libs -DCRYPTO_ENGINE_PATH=libsodium ssession.c -lthemis -lsoter -lsodium -o ssession-libsodium

ssession-openssl: ssession.c
	gcc -I../src/ -L../build ssession.c -lthemis -lsoter -lcrypto -o ssession-openssl

scell.csv: scell-libsodium scell-openssl
	./benchmarks.sh scell 20000

smessage.csv: smessage-libsodium smessage-openssl
	./benchmarks.sh smessage 30000

ssession.csv: ssession-libsodium ssession-openssl
	./benchmarks.sh ssession 40000

png: scell.csv smessage.csv ssession.csv
	gnuplot scell.src >scell.png
	gnuplot smessage.src >smessage.png
	gnuplot ssession.src >ssession.png

clean:
	rm *.csv
	rm *.png
	rm scell-libsodium
	rm scell-openssl
	rm smessage-libsodium
	rm smessage-openssl
	rm ssession-libsodium
	rm ssession-openssl