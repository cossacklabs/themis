CC=clang -framework Foundation
INC_PATH=-I../../../ -I../../../../libs/librebin/include
LIB_PATH=-L../../../../build -L../../../../libs/librebin/lib
LIBS=-lcrypto
#OBJC_PARAM= -I `gnustep-config --variable=GNUSTEP_SYSTEM_HEADERS` \
#            -L `gnustep-config --variable=GNUSTEP_SYSTEM_LIBRARIES` \
#            -lgnustep-base -fconstant-string-class=NSConstantString \
#            -D_NATIVE_OBJC_EXCEPTIONS \
#            -lobjc

SRCS := $(wildcard objcthemis/*.m)
OBJS := $(patsubst objcthemis/%.m,objcthemis/%.o, $(SRCS))

test: 
	$(CC) -c smessage_test.m $(OBJC_PARAM) $(INC_PATH) $(LIB_PATH) $(LIBS)
	$(CC) -o smessage smessage_test.o objcthemis.a ../../../../build/libthemis.a ../../../../build/libsoter.a $(OBJC_PARAM) $(INC_PATH) $(LIB_PATH) $(LIBS)
	$(CC) -c ssession_nsurlsession_wrapper.m $(OBJC_PARAM) $(INC_PATH) $(LIB_PATH) $(LIBS)
	$(CC) -c ssession_test.m $(OBJC_PARAM) $(INC_PATH) $(LIB_PATH) $(LIBS)
	$(CC) -o ssesion ssession_test.o ssession_nsurlsession_wrapper.o objcthemis.a ../../../../build/libthemis.a ../../../../build/libsoter.a $(OBJC_PARAM) $(INC_PATH) $(LIB_PATH) $(LIBS)


static: $(OBJS)
	$(AR) rcs objcthemis.a $(OBJS)

%.o: %.m
	$(CC)  $(OBJC_PARAM) $(INC_PATH) $(LIB_PATH) $(LIBS) -c $< -o $@

clean:
	rm *.a *.o
	rm objcthemis/*.o