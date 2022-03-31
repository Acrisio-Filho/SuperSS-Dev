CC := g++-10

PRGNAME := Linux\ IOCP

CFLAGS := -c -Wall -fpermissive -fsigned-char -std=c++20 `pkg-config --cflags glib-2.0` 
#-fpermissive
CLIBS := -pthread -lg

VMAIN := IOCP\ Linux/main.cpp
VIOCP := IOCP/iocp.cpp
VEXCEPTION := ../Projeto\ IOCP/UTIL/exception.cpp
VHEX_UTIL := ../Projeto\ IOCP/UTIL/hex_util.cpp
VTHREAD := ../Projeto\ IOCP/THREAD\ POOL/thread.cpp
VMESSAGE := ../Projeto\ IOCP/UTIL/message.cpp
VMESSAGE_POOL := ../Projeto\ IOCP/UTIL/message_pool.cpp
VUTIL_TIME := ../Projeto\ IOCP/UTIL/util_time.cpp
VWIN_PORT := ../Projeto\ IOCP/UTIL/WinPort.cpp
VREADER_INI := ../Projeto\ IOCP/UTIL/reader_ini.cpp
VSESSION := ../Projeto\ IOCP/SOCKET/session.cpp
VPACKET := ../Projeto\ IOCP/PACKET/packet.cpp
VBUFFER := ../Projeto\ IOCP/UTIL/buffer.cpp
VCRYPT := ../Projeto\ IOCP/CRYPT/crypt.cpp
VCOMPRESS := ../Projeto\ IOCP/COMPRESS/compress.cpp
VMINILZO := ../Projeto\ IOCP/COMPRESS/minilzo.c

#Object GLIB 2.64.6
O_GLIB-2_0 := lglib-2.0

all: todo

todo: main.o iocp.o exception.o hex_util.o thread.o message.o message_pool.o util_time.o WinPort.o reader_ini.o session.o packet.o buffer.o crypt.o compress.o minilzo.o
	$(CC) $(CLIBS) main.o iocp.o exception.o hex_util.o thread.o message.o message_pool.o util_time.o WinPort.o reader_ini.o session.o packet.o buffer.o crypt.o compress.o minilzo.o $(O_GLIB-2_0) -o $(PRGNAME)

main.o: $(VMAIN)
	$(CC) $(CFLAGS) $(VMAIN)

iocp.o: $(VIOCP)
	$(CC) $(CFLAGS) $(VIOCP)

exception.o: $(VEXCEPTION)
	$(CC) $(CFLAGS) $(VEXCEPTION)

hex_util.o: $(VHEX_UTIL)
	$(CC) $(CFLAGS) $(VHEX_UTIL)

thread.o: $(VTHREAD)
	$(CC) $(CFLAGS) $(VTHREAD)

message.o: $(VMESSAGE)
	$(CC) $(CFLAGS) $(VMESSAGE)

message_pool.o: $(VMESSAGE_POOL)
	$(CC) $(CFLAGS) $(VMESSAGE_POOL)

util_time.o: $(VUTIL_TIME)
	$(CC) $(CFLAGS) $(VUTIL_TIME)

WinPort.o: $(VWIN_PORT)
	$(CC) $(CFLAGS) $(VWIN_PORT)

reader_ini.o: $(VREADER_INI)
	$(CC) $(CFLAGS) $(VREADER_INI)

session.o: $(VSESSION)
	$(CC) $(CFLAGS) $(VSESSION)

packet.o: $(VPACKET)
	$(CC) $(CFLAGS) $(VPACKET)

buffer.o: $(VBUFFER)
	$(CC) $(CFLAGS) $(VBUFFER)

crypt.o: $(VCRYPT)
	$(CC) $(CFLAGS) $(VCRYPT)

compress.o: $(VCOMPRESS)
	$(CC) $(CFLAGS) $(VCOMPRESS)

minilzo.o: $(VMINILZO)
	$(CC) $(CFLAGS) $(VMINILZO)

clean:
	rm -rf *.o

clean-all:
	rm -rf *.o $(PRGNAME)

# testes
tmd5: testmd5.cpp
	g++-10 -ggdb -lg -pthread -std=c++20 testmd5.cpp ../Projeto\ IOCP/UTIL/md5.cpp ../Projeto\ IOCP/UTIL/exception.cpp \
	../Projeto\ IOCP/UTIL/message_pool.cpp ../Projeto\ IOCP/UTIL/message.cpp ../Projeto\ IOCP/UTIL/hex_util.cpp \
	../Projeto\ IOCP/UTIL/util_time.cpp ../Projeto\ IOCP/UTIL/WinPort.cpp -lcrypto -o testmd5

# -Wall -Wextra
teste: test.cpp
	g++-10 -ggdb -pthread -lg -std=c++20 `pkg-config --cflags glib-2.0` test.cpp \
	../Projeto\ IOCP/UTIL/exception.cpp ../Projeto\ IOCP/UTIL/message_pool.cpp \
	../Projeto\ IOCP/UTIL/message.cpp ../Projeto\ IOCP/TIMER/queue_timer.cpp ../Projeto\ IOCP/UTIL/event.cpp \
	../Projeto\ IOCP/THREAD\ POOL/thread.cpp ../Projeto\ IOCP/UTIL/WinPort.cpp ../Projeto\ IOCP/UTIL/reader_ini.cpp \
	../Projeto\ IOCP/UTIL/util_time.cpp ../Projeto\ IOCP/THREAD\ POOL/job_pool.cpp \
	../Projeto\ IOCP/THREAD\ POOL/job.cpp ../Projeto\ IOCP/TIMER/timer.cpp \
	../Projeto\ IOCP/TIMER/timer_manager.cpp ../Projeto\ IOCP/UTIL/hex_util.cpp \
	../Projeto\ IOCP/THREAD\ POOL/threadpool.cpp ../Projeto\ IOCP/IOCP/epoll.cpp \
	../Projeto\ IOCP/UTIL/buffer.cpp ../Projeto\ IOCP/SOCKET/session.cpp ../Projeto\ IOCP/PACKET/packet.cpp \
	../Projeto\ IOCP/COMPRESS/compress.cpp ../Projeto\ IOCP/COMPRESS/minilzo.c ../Projeto\ IOCP/CRYPT/crypt.cpp \
	../Projeto\ IOCP/Server/server.cpp ../Projeto\ IOCP/THREAD\ POOL/threadpl_server.cpp \
	../Projeto\ IOCP/DATABASE/normal_manager_db.cpp ../Projeto\ IOCP/DATABASE/normal_db.cpp \
	../Projeto\ IOCP/DATABASE/response.cpp ../Projeto\ IOCP/DATABASE/result_set.cpp \
	../Projeto\ IOCP/SOCKET/socketserver.cpp ../Projeto\ IOCP/SOCKET/socket.cpp \
	../Projeto\ IOCP/PANGYA_DB/pangya_db.cpp ../Projeto\ IOCP/PANGYA_DB/cmd_register_server.cpp \
	../Projeto\ IOCP/PANGYA_DB/cmd_server_list.cpp ../Projeto\ IOCP/PANGYA_DB/cmd_list_ip_ban.cpp \
	../Projeto\ IOCP/PANGYA_DB/cmd_list_mac_ban.cpp ../Projeto\ IOCP/SOCKET/session_manager.cpp \
	../Projeto\ IOCP/PACKET/packet_func.cpp ../Projeto\ IOCP/UTIL/func_arr.cpp \
	../Projeto\ IOCP/Smart\ Calculator/Smart\ Calculator.cpp ../Projeto\ IOCP/DATABASE/exec_query.cpp \
	../Projeto\ IOCP/DATABASE/database.cpp ../Projeto\ IOCP/UNIT/unit_auth_server_connect.cpp \
	../Projeto\ IOCP/UNIT/unit_connect.cpp ../Projeto\ IOCP/PANGYA_DB/cmd_new_auth_server_key.cpp \
	../Projeto\ IOCP/DATABASE/mssql.cpp ../Projeto\ IOCP/DATABASE/postgresql.cpp -ldl -lodbc `pkg-config --libs glib-2.0` -o test