# Makefile felix3

# <one line to give the program's name and a brief idea of what it does.>
# Copyright (C) 2019  Cristian Andrione.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

CC=gcc
CFLAGS=-DWITH_OPENSSL
#CFLAGS=-DWITH_OPENSSL -m32
#CFLAGS=-DWITH_OPENSSL -Wall -g
CLIBS=-lssl -lcrypto -lgsoapssl

S=src/
OSOAP=$(S)soapC.o $(S)soapClient.o
OBJS=$(S)guarda_cae.o $(S)expira_ta.o $(S)extrae_sign.o $(S)extrae_token.o $(S)procesa_cuit.o
SRCSOAP=$(S)soapC.c $(S)soapClient.c
SRC=$(S)guarda_cae.c $(S)expira_ta.c $(S)extrae_sign.c $(S)extrae_token.c $(S)procesa_cuit.c
SRC_CONSUL=$(S)consul_cae.c $(S)expira_ta.c $(S)extrae_sign.c $(S)extrae_token.c $(S)procesa_cuit.c

all: genera_tra.exe recibe_args.exe wsaa.exe wsfe.exe\
	consulta.exe ulti.exe

#EJECUTABLES

genera_tra.exe: $(S)genera_tra.o
	$(CC) -o genera_tra.exe $(S)genera_tra.o

recibe_args.exe: $(S)recibe_args.o
	$(CC) -o recibe_args.exe $(S)recibe_args.o

wsaa.exe: $(S)wsaa.o $(OSOAP)
	$(CC) $(CFLAGS) -o wsaa.exe $(S)wsaa.o\
		$(SRCSOAP) $(CLIBS)

wsfe.exe: $(S)wsfe.o $(OBJS) $(OSOAP)
	$(CC) $(CFLAGS) -o wsfe.exe $(S)wsfe.o\
		$(SRC) $(SRCSOAP) $(CLIBS)

consulta.exe: $(S)consulta.o $(OBJS) $(OSOAP)
	$(CC) $(CFLAGS) -o consulta.exe $(S)consulta.o\
		$(SRC_CONSUL) $(SRCSOAP) $(CLIBS)

ulti.exe: $(S)ulti.o $(OBJS) $(OSOAP)
	$(CC) $(CFLAGS) -o ulti.exe $(S)ulti.o\
		$(SRC_CONSUL) $(SRCSOAP) $(CLIBS)

#OBJETOS

$(S)genera_tra.o: $(S)genera_tra.c
	$(CC) -o $(S)genera_tra.o -c $(S)genera_tra.c

$(S)recibe_args.o: $(S)recibe_args.c
	$(CC) -o $(S)recibe_args.o -c $(S)recibe_args.c

$(S)wsaa.o: $(S)wsaa.c
	$(CC) -o $(S)wsaa.o -c $(S)wsaa.c

$(S)wsfe.o: $(S)wsfe.c
	$(CC) $(CFLAGS) -o $(S)wsfe.o -c $(S)wsfe.c

$(S)consulta.o: $(S)consulta.c
	$(CC) $(CFLAGS) -o $(S)consulta.o -c $(S)consulta.c

$(S)ulti.o: $(S)ulti.c
	$(CC) $(CFLAGS) -o $(S)ulti.o -c $(S)ulti.c

#FUENTES SOAP

$(S)soapC.o: $(S)soapC.c
	$(CC) -o $(S)soapC.o -c $(S)soapC.c

$(S)soapClient.o: $(S)soapClient.c
	$(CC) -o $(S)soapClient.o -c $(S)soapClient.c

#FUENTES SRC
$(S)consul_cae.o: $(S)consul_cae.c
	$(CC) -o $(S)consul_cae.o -c $(S)consul_cae.c

$(S)guarda_cae.o: $(S)guarda_cae.c
	$(CC) -o $(S)guarda_cae.o -c $(S)guarda_cae.c

$(S)expira_ta.o: $(S)expira_ta.c
	$(CC) -o $(S)expira_ta.o -c $(S)expira_ta.c

$(S)extrae_sign.o: $(S)extrae_sign.c
	$(CC) -o $(S)extrae_sign.o -c $(S)extrae_sign.c

$(S)extrae_token.o: $(S)extrae_token.c
	$(CC) -o $(S)extrae_token.o -c $(S)extrae_token.c

$(S)procesa_cuit.o: $(S)procesa_cuit.c
	$(CC) -o $(S)procesa_cuit.o -c $(S)procesa_cuit.c


.PHONY: clean cleanall genera genera_homo

clean:
	rm -f $(S)*.o $(S)*.xml *~ core
	rm -f *.o *~ core

cleanall:
	rm -f $(S)*.o $(S)*.h $(S)*.xml $(S)soap* $(S)*.nsmap
	rm -f cae* err* tra* ta*.xml *.exe *.h *.o *~ core Logs/*

genera:
	cd $(S);\
	wsdl2h -c -o wsfe.h wsaa_prod.wsdl wsfe_prod.wsdl;\
	soapcpp2 -c -CL wsfe.h

genera_homo:
	cd $(S);\
	wsdl2h -c -o wsfe.h wsaa_homo.wsdl wsfe_homo.wsdl;\
	soapcpp2 -c -CL wsfe.h
