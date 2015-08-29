#include "mbed.h"
#include "EthernetInterface.h"
//#include "SDFileSystem.h"
#include "FreescaleIAP.h"
#include <string>

#define Ip "192.168.20.3"
#define Mask "255.255.255.0"
#define Gateway "192.168.20.1"

//SDFileSystem sd(PTE3, PTE1, PTE2, PTE4, "sd");

Serial PC(USBTX, USBRX);

EthernetInterface Net;

char buffer[]= "";
int SPin;
char Var[3];
int Mode[43], Type[43],a=0,t=0;
int Enable[43],data[4];
int Pin[43];
int ADC=0;
int address = flash_size() - SECTOR_SIZE; //Write in last sector
int *Config = (int*)address;
static unsigned int ADCP32, ADCP33, ADCP34, ADCP35, ADCP36, ADCP37;
unsigned char MSB32[2], MSB33[2], MSB34[2], MSB35[2], MSB36[2], MSB37[2];

static const PinName PinM[43] = {PTC5, PTC7, PTC0, PTC9, PTC8,PTC1, PTB19, PTB18, PTC3, PTC2, PTA2, PTB23, PTA1, PTB9, PTC17, PTC16, PTE24, PTE25, PTD1, PTD3, PTD2, PTD0, PTC4, PTA0};
char State[32], SPP0[1], SPP1[1], SPP2[1], SPP3[1];
int SP0=0, SP1=0, SP2=0, SP3=0;

//Dichiarazione dei Pin 
DigitalInOut P0(PinM[0]);
DigitalInOut P1(PinM[1]);
DigitalInOut P2(PinM[2]);
DigitalInOut P3(PinM[3]);
DigitalInOut P4(PinM[4]);
DigitalInOut P5(PinM[5]);
DigitalInOut P6(PinM[6]);
DigitalInOut P7(PinM[7]);
DigitalInOut P8(PinM[8]);
DigitalInOut P9(PinM[9]);
DigitalInOut P10(PinM[10]);
DigitalInOut P11(PinM[11]);
DigitalInOut P12(PinM[12]);
DigitalInOut P13(PinM[13]);
DigitalInOut P14(PinM[14]);
DigitalInOut P15(PinM[15]);
/*DigitalInOut P16(ADC0_DM1);
DigitalInOut P17(PTA16);
DigitalInOut P18(PTA17);
DigitalInOut P19(PTA18);
DigitalInOut P20(PTB20);
DigitalInOut P21(DAC0_OUT);
DigitalInOut P22(PTA19);
DigitalInOut P23(PTA20);*/
DigitalInOut P24(PinM[16]);
DigitalInOut P25(PinM[17]);
DigitalInOut P26(PinM[18]);
DigitalInOut P27(PinM[19]);
DigitalInOut P28(PinM[20]);
DigitalInOut P29(PinM[21]);
DigitalInOut P30(PinM[22]);
DigitalInOut P31(PinM[23]);
AnalogIn     P32(PTB2);
AnalogIn     P33(PTB3);
AnalogIn     P34(PTB10);
AnalogIn     P35(PTB11);
AnalogIn     P36(PTC11);
AnalogIn     P37(PTC10);


int main ()
{

    PC.baud(9600); //configuro la seriale per PC ad un baud rate di 9600 
    PC.printf("\n\rEthernet project\n\r");


    /*sd.mount();

    if(!sd.disk_initialize())PC.printf("SD initailize\n\r");
    else PC.printf("SD not inizialized\n\r");

    FILE *fp=fopen("/sd/settings.txt", "r");

    if(fp != NULL) {
        while(1) {
            t=fgetc(fp);
            if(t=='\n')break;
            IP+=t;
        }
        while(1) {
            t=fgetc(fp);
            if(t=='\n')break;
            MASK+=t;
        }
        while(1) {
            t=fgetc(fp);
            if(t=='\n')break;
            GATEWAY+=t;
        }
        PC.printf("IP: %s\n\r", IP.c_str());
        PC.printf("MASK: %s\n\r", MASK.c_str());
        PC.printf("GATEWAY: %s\n\r", GATEWAY.c_str());

        while(1) {
            a++;
            Enable[a]=fgetc(fp);
            if(Enable[a]== EOF) break;

            Var[0]=fgetc(fp);
            Type[a]=fgetc(fp);

            Var[0]=fgetc(fp);
            Var[0]=fgetc(fp);
            Var[1]=fgetc(fp);
            if(Var[1]==',') {
                Pin[a]=(Var[0] - 48);
                //PC.printf("%d ", Pin[a]);
            } else {
                Pin[a]=(Var[0] - 48)*10+(Var[1] - 48);
                //PC.printf("%d ", Pin[a]);
                Var[0]=fgetc(fp);
            }

            Mode[a]=fgetc(fp);

            Var[0]=fgetc(fp);

            if(Type[a]!='A' && Type[a]!='D' && Type[a]!='P') {
                Enable[a]=0;
            }
            PC.printf("%d ",Enable[a]);
            PC.printf("%c ", Type[a]);
            PC.printf("%d ", Pin[a]);
            PC.printf("%c \n\r", Mode[a]);


        }
    } else {

        PC.printf("Errore lettura SD\n\r");
    }

    sd.unmount();*/



    PC.printf("Configurazione ethernet\n\r");

    TCPSocketServer Server; 

    TCPSocketConnection Client;

    //configuro la porta ethernet con indirizzo IP, Mask e Gateway
    if(!Net.init(Ip, Mask, Gateway)) {
        PC.printf("Ethernet configured IP: %s\n\r", Net.getIPAddress());
    } else PC.printf("Ethernet not configured\n\r");
    
    //Attendo un massimo di 2 secondi per la connessione 
    if(!Net.connect(2000)) {
        PC.printf("Ethernet connected IP: %s\n\r", Net.getIPAddress());
    } else PC.printf("Ethernet not connected\n\r");

    //Configuro la porta per la comunicazione
    if(!Server.bind(50)) {
        PC.printf("Bind succeded\n\r");
    } else {
        PC.printf("Bind fail\n\r");
    }

    if(!Server.listen()) {
        PC.printf("Listening\n\r");
    } else {
        PC.printf("Listening fail\n\r");
    }

    //Disabilito il blocco dei socket da parte del server
    Server.set_blocking(false, 1500);

    while(true) {
        //Controllo se ci sono richieste di connessione da parte di un client
        if(!Server.accept(Client)) {
            PC.printf("New client IP: %s\n\r", Client.get_address());
        } else {
            PC.printf("No client\n\r");
        }
        
        //disabilto il blocco dei soket da parte del client 
        Client.set_blocking(false, 2);

        while(true) {
            //eseguo un ciclo infinito fino a che ricevo dati dal client e li salvo in un buffer
            int n = Client.receive_all(buffer, 10);
            if (n <= 0) break;
            PC.printf("Received message from Client :'%s' \n\rRecived: %d byte\n\r", buffer, n);
            //controllo se è presente la configurazione dei banchi o se sono configurati correttamente
            if(Config[0] != '0' && Config[0] != 'F')
                Client.send_all("Banco 1 non configurato", 23);
            if(Config[1] != '0' && Config[1] != 'F')
                Client.send_all("Banco 2 non configurato", 23);
            if(Config[2] != '0' && Config[2] != 'F')
                Client.send_all("Banco 3 non configurato", 23);
            if(Config[3] != '0' && Config[3] != 'F')
                Client.send_all("Banco 4 non configurato", 23);
            //Se è presente 'CON' prima della configurazione aggiorno la configurazione dei banchi e nel caso non sia presente la creo 
            if(buffer[0]== 'C' && buffer[1] == 'O' && buffer[2] == 'N') {
                for(int j=0; j<4; j++)
                    data[j]=buffer[j+3];
                printf("Starting\r\n");
                erase_sector(address);
                int numbers[4] = {data[0], data[1], data[2], data[3]};
                program_flash(address, (char*)&numbers, 16);
                printf("Resulting flash: \r\n");
                for (int i = 0; i<4; i++)
                    printf("%c\r\n", Config[i]);

                printf("Done\r\n\n");

                buffer[0]=0;
                buffer[1]=0;
                buffer[2]=0;
                buffer[3]=0;
                buffer[4]=0;
                buffer[5]=0;
                buffer[6]=0;

            }
            
            
            
            //ricavo il pin dalle due posizione dei buffer 
            SPin = ((buffer[2] - 48) * 10) + (buffer[3] - 48);

            PC.printf("%d\n\r", SPin);
            
            //controllo quale pin è stato selezionato
            switch(SPin) {
                case 0:
                    //Controllo se sul pin si può leggere lo stato oppure scrivere sul pin, in base alla configurazione.
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[0] == 'F') {
                        P0.output();
                        P0 = (buffer[4]- 48);
                        State[0] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[0] == '0') {
                        P0.input();
                        if(P0.read())Client.send_all("P0=1", 4);
                        else Client.send_all("P0=0", 4);
                        PC.printf("Lettura\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'B'){
                        SPP0[0]=SP0;
                        Client.send_all(SPP0, 1);
                    }else
                        Client.send_all("Operazione non consentita", 25);
                    break;

                case 1:
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[0] == 'F') {
                        P1.output();
                        P1 = (buffer[4]- 48);
                        State[1] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[0] == '0') {
                        P1.input();
                        if(P1.read())Client.send_all("P1=1", 4);
                        else Client.send_all("P1=0", 4);
                        PC.printf("Lettura\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'B'){                    
                        SPP1[0]=SP1;
                        Client.send(SPP1, 1);
                    }else
                        Client.send_all("Operazione non consentita", 25);
                    
                    break;

                case 2:
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[0] == 'F') {
                        P2.output();
                        P2 = (buffer[4]- 48);
                        State[2] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[0] == '0') {
                        P2.input();
                        if(P2.read())Client.send_all("P2=1", 4);
                        else Client.send_all("P2=0", 4);
                        PC.printf("Lettura\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'B'){                    
                        SPP2[0]=SP2;
                        Client.send(SPP2, 1);
                    } else
                        Client.send_all("Operazione non consentita", 25);
                    break;

                case 3:
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[0] == 'F') {
                        P3.output();
                        P3 = (buffer[4]- 48);
                        State[3] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[0] == '0') {
                        P3.input();
                        if(P3.read())Client.send_all("P3=1", 4);
                        else Client.send_all("P3=0", 4);
                        PC.printf("Lettura\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'B'){                    
                        SPP3[0]=SP3;
                        Client.send(SPP3, 1);
                    }else
                        Client.send_all("Operazione non consentita", 25);
                    break;

                case 4:
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[0] == 'F') {
                        P4.output();
                        P4 = (buffer[4]- 48);
                        State[4] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[0] == '0') {
                        P4.input();
                        if(P4.read())Client.send_all("P4=1", 4);
                        else Client.send_all("P4=0", 4);
                        PC.printf("Lettura\n\r");
                    } else Client.send_all("Operazione non consentita", 25);
                    break;

                case 5:
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[0] == 'F') {
                        P5.output();
                        P5 = (buffer[4]- 48);
                        State[5] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[0] == '0') {
                        P5.input();
                        if(P5.read())Client.send_all("P5=1", 4);
                        else Client.send_all("P5=0", 4);
                        PC.printf("Lettura\n\r");
                    } else Client.send_all("Operazione non consentita", 25);
                    break;

                case 6:
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[0] == 'F') {
                        P6.output();
                        P6 = (buffer[4]- 48);
                        State[6] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[0] == '0') {
                        P6.input();
                        if(P6.read())Client.send_all("P6=1", 4);
                        else Client.send_all("P6=0", 4);
                        PC.printf("Lettura\n\r");
                    } else Client.send_all("Operazione non consentita", 25);
                    break;

                case 7:
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[0] == 'F') {
                        P7.output();
                        P7 = (buffer[4]- 48);
                        State[7] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[0] == '0') {
                        P7.input();
                        if(P7.read())Client.send_all("P7=1", 4);
                        else Client.send_all("P0=0", 4);
                        PC.printf("Lettura\n\r");
                    } else Client.send_all("Operazione non consentita", 25);
                    break;

                case 8:
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[1] == 'F') {
                        P8.output();
                        P8 = (buffer[4]- 48);
                        State[8] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[1] == '0') {
                        P8.input();
                        if(P8.read())Client.send_all("P8=1", 4);
                        else Client.send_all("P8=0", 4);
                        PC.printf("Lettura\n\r");
                    } else Client.send_all("Operazione non consentita", 25);
                    break;

                case 9:
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[1] == 'F') {
                        P9.output();
                        P9 = (buffer[4]- 48);
                        State[9] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[1] == '0') {
                        P9.input();
                        if(P9.read())Client.send_all("P9=1", 4);
                        else Client.send_all("P9=0", 4);
                        PC.printf("Lettura\n\r");
                    } else Client.send_all("Operazione non consentita", 25);
                    break;

                case 10:
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[1] == 'F') {
                        P10.output();
                        P10 = (buffer[4]- 48);
                        State[10] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[1] == '0') {
                        P10.input();
                        if(P10.read())Client.send_all("P10=1", 5);
                        else Client.send_all("P10=0", 5);
                        PC.printf("Lettura\n\r");
                    } else Client.send_all("Operazione non consentita", 25);
                    break;

                case 11:
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[1] == 'F') {
                        P11.output();
                        P11 = (buffer[4]- 48);
                        State[11] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[1] == '0') {
                        P11.input();
                        if(P11.read())Client.send_all("P11=1", 5);
                        else Client.send_all("P11=0", 5);
                        PC.printf("Lettura\n\r");
                    } else Client.send_all("Operazione non consentita", 25);
                    break;

                case 12:
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[1] == 'F') {
                        P12.output();
                        P12 = (buffer[4]- 48);
                        State[12] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[1] == '0') {
                        P12.input();
                        if(P12.read())Client.send_all("P12=1", 5);
                        else Client.send_all("P12=0", 5);
                        PC.printf("Lettura\n\r");
                    } else Client.send_all("Operazione non consentita", 25);
                    break;

                case 13:
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[1] == 'F') {
                        P13.output();
                        P13 = (buffer[4]- 48);
                        State[13] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[1] == '0') {
                        P13.input();
                        if(P13.read())Client.send_all("P13=1", 4);
                        else Client.send_all("P13=0", 4);
                        PC.printf("Lettura\n\r");
                    } else Client.send_all("Operazione non consentita", 25);
                    break;

                case 14:
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[1] == 'F') {
                        P14.output();
                        P14 = (buffer[4]- 48);
                        State[14] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[1] == '0') {
                        P14.input();
                        if(P14.read())Client.send_all("P14=1", 4);
                        else Client.send_all("P14=0", 4);
                        PC.printf("Lettura\n\r");
                    } else Client.send_all("Operazione non consentita", 25);
                    break;

                case 15:
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[1] == 'F') {
                        P15.output();
                        P15 = (buffer[4]- 48);
                        State[15] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[1] == '0') {
                        P15.input();
                        if(P15.read())Client.send_all("P15=1", 4);
                        else Client.send_all("P15=0", 4);
                        PC.printf("Lettura\n\r");
                    } else Client.send_all("Operazione non consentita", 25);
                    break;

                    /*case 16:
                        if(buffer[0] == 'W' && buffer[1] == 'D' && Config[2] == 'F') {
                            P16.output();
                            P16 = (buffer[4]- 48);
                            PC.printf("Attivo\n\r");
                        } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[2] == '0') {
                            P16.input();
                            if(P16.read())Client.send_all("P16=1", 4);
                            else Client.send_all("P16=0", 4);
                            PC.printf("Lettura\n\r");
                        }else Client.send_all("Operazione non consentita", 25);
                        break;

                    case 17:
                        if(buffer[0] == 'W' && buffer[1] == 'D' && Config[2] == 'F') {
                            P17.output();
                            P17 = (buffer[4]- 48);
                            PC.printf("Attivo\n\r");
                        } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[2] == '0') {
                            P17.input();
                            if(P17.read())Client.send_all("P17=1", 4);
                            else Client.send_all("P17=0", 4);
                            PC.printf("Lettura\n\r");
                        }else Client.send_all("Operazione non consentita", 25);
                        break;

                    case 18:
                        if(buffer[0] == 'W' && buffer[1] == 'D' && Config[2] == 'F') {
                            P18.output();
                            P18 = (buffer[4]- 48);
                            PC.printf("Attivo\n\r");
                        } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[2] == '0') {
                            P18.input();
                            if(P18.read())Client.send_all("P18=1", 4);
                            else Client.send_all("P18=0", 4);
                            PC.printf("Lettura\n\r");
                        }else Client.send_all("Operazione non consentita", 25);
                        break;

                    case 19:
                        if(buffer[0] == 'W' && buffer[1] == 'D' && Config[2] == 'F') {
                            P19.output();
                            P19 = (buffer[4]- 48);
                            PC.printf("Attivo\n\r");
                        } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[2] == '0') {
                            P19.input();
                            if(P19.read())Client.send_all("P19=1", 4);
                            else Client.send_all("P19=0", 4);
                            PC.printf("Lettura\n\r");
                        }else Client.send_all("Operazione non consentita", 25);
                        break;

                    case 20:
                        if(buffer[0] == 'W' && buffer[1] == 'D' && Config[2] == 'F') {
                            P20.output();
                            P20 = (buffer[4]- 48);
                            PC.printf("Attivo\n\r");
                        } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[2] == '0') {
                            P20.input();
                            if(P20.read())Client.send_all("P20=1", 4);
                            else Client.send_all("P20=0", 4);
                            PC.printf("Lettura\n\r");
                        }else Client.send_all("Operazione non consentita", 25);
                        break;

                    case 21:
                        if(buffer[0] == 'W' && buffer[1] == 'D' && Config[2] == 'F') {
                            P21.output();
                            P21 = (buffer[4]- 48);
                            PC.printf("Attivo\n\r");
                        } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[2] == '0') {
                            P21.input();
                            if(P21.read())Client.send_all("P21=1", 5);
                            else Client.send_all("P21=0", 5);
                            PC.printf("Lettura\n\r");
                        }else Client.send_all("Operazione non consentita", 25);
                        break;

                    case 22:
                        if(buffer[0] == 'W' && buffer[1] == 'D' && Config[2] == 'F') {
                            P22.output();
                            P22 = (buffer[4]- 48);
                            PC.printf("Attivo\n\r");
                        } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[2] == '0') {
                            P22.input();
                            if(P22.read())Client.send_all("P22=1", 5);
                            else Client.send_all("P22=0", 5);
                            PC.printf("Lettura\n\r");
                        }else Client.send_all("Operazione non consentita", 25);
                        break;

                    case 23:
                        if(buffer[0] == 'W' && buffer[1] == 'D' && Config[2] == 'F') {
                            P23.output();
                            P23 = (buffer[4]- 48);
                            PC.printf("Attivo\n\r");
                        } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[2] == '0') {
                            P23.input();
                            if(P23.read())Client.send_all("P23=1", 5);
                            else Client.send_all("P23=0", 5);
                            PC.printf("Lettura\n\r");
                        }else Client.send_all("Operazione non consentita", 25);
                        break;*/

                case 24:
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[3] == 'F') {
                        P24.output();
                        P24 = (buffer[4]- 48);
                        State[24] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[3] == '0') {
                        P24.input();
                        if(P24.read())Client.send_all("P24=1", 5);
                        else Client.send_all("P24=0", 5);
                        PC.printf("Lettura\n\r");
                    } else Client.send_all("Operazione non consentita", 25);
                    break;

                case 25:
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[3] == 'F') {
                        P25.output();
                        P25 = (buffer[4]- 48);
                        State[25] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[3] == '0') {
                        P25.input();
                        if(P25.read())Client.send_all("P0=1", 5);
                        else Client.send_all("P0=0", 5);
                        PC.printf("Lettura\n\r");
                    } else Client.send_all("Operazione non consentita", 25);
                    break;

                case 26:
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[3] == 'F') {
                        P26.output();
                        P26 = (buffer[4]- 48);
                        State[26] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[3] == '0') {
                        P26.input();
                        if(P26.read())Client.send_all("P26=1", 5);
                        else Client.send_all("P26=0", 5);
                        PC.printf("Lettura\n\r");
                    } else Client.send_all("Operazione non consentita", 25);
                    break;

                case 27:
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[3] == 'F') {
                        P27.output();
                        P27 = (buffer[4]- 48);
                        State[27] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[3] == '0') {
                        P27.input();
                        if(P27.read())Client.send_all("P27=1", 5);
                        else Client.send_all("P27=0", 5);
                        PC.printf("Lettura\n\r");
                    } else Client.send_all("Operazione non consentita", 25);
                    break;

                case 28:
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[3] == 'F') {
                        P28.output();
                        P28 = (buffer[4]- 48);
                        State[28] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[3] == '0') {
                        P28.input();
                        if(P28.read())Client.send_all("P28=1", 5);
                        else Client.send_all("P28=0", 5);
                        PC.printf("Lettura\n\r");
                    } else Client.send_all("Operazione non consentita", 25);
                    break;

                case 29:
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[3] == 'F') {
                        P29.output();
                        P29 = (buffer[4]- 48);
                        State[29] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[3] == '0') {
                        P29.input();
                        if(P29.read())Client.send_all("P29=1", 5);
                        else Client.send_all("P29=0", 5);
                        PC.printf("Lettura\n\r");
                    } else Client.send_all("Operazione non consentita", 25);
                    break;

                case 30:
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[3] == 'F') {
                        P30.output();
                        P30 = (buffer[4]- 48);
                        State[30] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[3] == '0') {
                        P30.input();
                        if(P30.read())Client.send_all("P30=1", 5);
                        else Client.send_all("P30=0", 5);
                        PC.printf("Lettura\n\r");
                    } else Client.send_all("Operazione non consentita", 25);
                    break;

                case 31:
                    if(buffer[0] == 'W' && buffer[1] == 'D' && Config[3] == 'F') {
                        P31.output();
                        P31 = (buffer[4]- 48);
                        State[31] = buffer[4] - 48;
                        PC.printf("Attivo\n\r");
                    } else if(buffer[0] == 'R' && buffer[1] == 'D' && Config[3] == '0') {
                        P31.input();
                        if(P31.read())Client.send_all("P31=1", 5);
                        else Client.send_all("P31=0", 5);
                        PC.printf("Lettura\n\r");
                    } else Client.send_all("Operazione non consentita\n", 25);
                    break;
                //per i pin Analogici divido in due parti la lettura dell'ADC e carico le due parti su un vettore per essere trasmesse in ethernet
                case 32:
                    if(buffer[0] == 'R' && buffer[1] == 'A') {
                        PC.printf("%d\n\r", ADCP32);
                        MSB32[0]=(char)ADCP32>>8;
                        MSB32[1]=(char)ADCP32 & 0x00FF;
                        Client.send_all((char*)MSB32, 5);
                    }else
                        Client.send_all("Operazione non consentita", 25);
                    break;

                case 33:
                    if(buffer[0] == 'R' && buffer[1] == 'A') {
                        PC.printf("%d\n\r", ADCP33);
                        MSB33[0]=(char)ADCP33>>8;
                        MSB33[1]=(char)ADCP33 & 0x00FF;           
                        Client.send((char*)MSB33, 5);
                    }else
                        Client.send_all("Operazione non consentita", 25);
                    break;

                case 34:
                    if(buffer[0] == 'R' && buffer[1] == 'A') {
                        PC.printf("%d\n\r", ADCP34);
                        MSB34[0]=(char)ADCP34>>8;
                        MSB34[1]=(char)ADCP34 & 0x00FF;
                        Client.send((char*)MSB34, 5);
                    }else
                        Client.send_all("Operazione non consentita", 25);
                    break;

                case 35:
                    if(buffer[0] == 'R' && buffer[1] == 'A') {
                        PC.printf("%d\n\r", ADCP35);
                        MSB35[0]=(char)ADCP35>>8;
                        MSB35[1]=(char)ADCP35 & 0x00FF;
                        Client.send((char*)MSB35, 5);
                    }else
                        Client.send_all("Operazione non consentita", 25);
                    break;

                case 36:
                    if(buffer[0] == 'R' && buffer[1] == 'A') {
                        PC.printf("%d\n\r", ADCP36);
                        MSB36[0]=(char)ADCP36>>8;
                        MSB36[1]=(char)ADCP36 & 0x00FF;
                        Client.send((char*)MSB36, 5);
                    }else
                        Client.send_all("Operazione non consentita", 25);
                    break;

                case 37:
                    if(buffer[0] == 'R' && buffer[1] == 'A') {
                        PC.printf("%d\n\r", ADCP37);
                        MSB37[0]=(char)ADCP37>>8;
                        MSB37[1]=(char)ADCP37 & 0x00FF;
                        Client.send((char*)MSB37, 5);
                    }else
                        Client.send_all("Operazione non consentita", 25);
                    break;

                //se non viene riconosciuto nessun pin restituisco al client un'errore.
                default:
                    Client.send_all("Errore pin non esistente", 24);
                    break;
            }
        }
        //eseguo ciclicamente le letture dell'ADC 
        ADCP32 = P32.read_u16();
        ADCP33 = P33.read_u16();
        ADCP34 = P34.read_u16();
        ADCP35 = P35.read_u16();
        ADCP36 = P36.read_u16();
        ADCP37 = P37.read_u16();
        
        SP0 = State[0] | State[1]<<1 | State[2]<<2 | State[3]<<3 | State[4]<<4 | State[5]<<5 | State[6]<<6 | State[7]<<7;
        SP1 = State[8] | State[9]<<1 | State[10]<<2 | State[11]<<3 | State[12]<<4 | State[13]<<5 | State[14]<<6 | State[15]<<7;
        SP2 = State[16] | State[17]<<1 | State[18]<<2 | State[19]<<3 | State[20]<<4 | State[21]<<5 | State[22]<<6 | State[23]<<7;
        SP3 = State[24] | State[25]<<1 | State[26]<<2 | State[27]<<3 | State[28]<<4 | State[29]<<5 | State[30]<<6 | State[31]<<7;       
    }
}