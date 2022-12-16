#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"

#define ADDRESS     "tcp://broker.emqx.io:1883"
#define CLIENTID    "broker.emqx.io"
#define TOPIC       "esp32/fum"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L
volatile MQTTClient_deliveryToken deliveredtoken;

void display();
void check(int square);
int checkWin(char square1, char square2, char square3);

char input[10]; 
int prgrmTurn = -1;
int endGame = -1;
int p1Win = -1;
int p2Win = -1;
char square[9] = {'-','-','-','-','-','-','-','-','-'};

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    deliveredtoken = dt;
}
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    char* payloadptr;
    printf("Message arrived\n");
    printf("    topic: %s\n", topicName);
    printf("    message: ");
    payloadptr = message->payload;

    payloadptr++;

    if(*payloadptr-- == '1')
    {
        if(*payloadptr == 'W')
        {
            p1Win = 1;
        } else if(*payloadptr == 'Q') {
            prgrmTurn = -1; //false
            endGame = 1;
        } else if(*payloadptr == '1') {
            square[0] = 'X';
            prgrmTurn = 1;
        } else if(*payloadptr == '2') {
            square[1] = 'X';
            prgrmTurn = 1;
        } else if(*payloadptr == '3') {
            square[2] = 'X';
            prgrmTurn = 1;
        } else if(*payloadptr == '4') {
            square[3] = 'X';
            prgrmTurn = 1;
        } else if(*payloadptr == '5') {
            square[4] = 'X';
            prgrmTurn = 1;
        } else if(*payloadptr == '6') {
            square[5] = 'X';
            prgrmTurn = 1;
        } else if(*payloadptr == '7') {
            square[6] = 'X';
            prgrmTurn = 1;
        } else if(*payloadptr == '8') {
            square[7] = 'X';
            prgrmTurn = 1;
        } else if(*payloadptr == '9') {
            square[8] = 'X';
            prgrmTurn = 1;
        }
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);

    return 1;
}
void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int main(int argc, char* argv[])
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;
    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    
    pubmsg.payload = PAYLOAD;
    pubmsg.payloadlen = strlen(PAYLOAD);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
    MQTTClient_subscribe(client, TOPIC, QOS);
    do
    {
        if(p2Win)
        {
            display();
            printf("Player 2 wins! Bye!");

            pubmsg.payload = input;
            pubmsg.payloadlen = strlen(input);
            pubmsg.qos = QOS;
            pubmsg.retained = 0;

            MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
            rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);

            return rc;
        }
        if(p1Win == 1)
        {
            display();
            printf("%s\n", "Player 1 Wins! Bye!");
            return rc;
        }

        if(endGame == 1)
        {
            printf("%s\n","Player 1 ended the game!");
            return rc;
        }

        if(prgrmTurn == -1)
        {
            display();
            printf("%s\n","Waiting for Player 1"); //Q to quit if waiting
        }
        else
        {
            display();
            printf("%s\n","Your turn!"); //1-9 or Q to quit
        }

        fflush(stdin);
        fgets(input,sizeof(input),stdin);

        if(input[0] == 'q' || input[0] == 'Q')
        {
            printf("\nYou Ended The Game\n");
            endGame = 1;
        }

        if(prgrmTurn && endGame != 1)
        {
            if(input[0] == '1') {
                check(0); 
            } else if(input[0] == '2') {
                check(1); 
            } else if(input[0] == '3') {
                check(2); 
            } else if(input[0] == '4') {
                check(3); 
            } else if(input[0] == '5') {
                check(4);               
            } else if(input[0] == '6') {
                check(5);
            } else if(input[0] == '7') {
                check(6);
            } else if(input[0] == '8') {
                check(7);
            } else if(input[0] == '9') {
                check(8);
            } else {
                if(prgrmTurn == 1)
                {
                    printf("Invalid input, please try again");
                }
            }            
        }

        input[1] = '2';

        pubmsg.payload = input;
        pubmsg.payloadlen = strlen(input);
        pubmsg.qos = QOS;
        pubmsg.retained = 0;

        MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
        rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    } while((input[0] != 'Q' && input[0] != 'q'));
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}

void display()
{
    printf("+-----------+ \n| %c | %c | %c | \n", square[0], square[1], square[2]);
    printf("+-----------+ \n| %c | %c | %c | \n", square[3], square[4], square[5]);
    printf("+-----------+ \n| %c | %c | %c | \n", square[6], square[7], square[8]);
    printf("+-----------+ \n\n");
}

void check(int box)
{
    if(square[box] == '-')
    {
        square[box] = 'O';
        prgrmTurn = -1;
    }
    else
    {
        printf("\nBox is occupied\n");
        input[0] = '0';
    }

    if (square[1] == square[2] && square[2] == square[3]) {
        if(square[1] != 'O' || square[2] != 'O' || square[3] != 'O')
        {
            p2Win = -1;
        } else {
            p2Win = 1;
        }
    } else if (square[4] == square[5] && square[5] == square[6]) {
        if(square[4] != 'O' || square[5] != 'O' || square[6] != 'O')
        {
            p2Win = -1;
        } else {
            p2Win = 1;
        } 
    } else if (square[7] == square[8] && square[8] == square[9]) {
        if(square[7] != 'O' || square[8] != 'O' || square[9] != 'O')
        {
            p2Win = -1;
        } else {
            p2Win = 1;
        }
    } else if (square[1] == square[4] && square[4] == square[7]) {
        if(square[1] != 'O' || square[4] != 'O' || square[7] != 'O')
        {
            p2Win = -1;
        } else {
            p2Win = 1;
        }  
    } else if (square[2] == square[5] && square[5] == square[8]) {
        if(square[2] != 'O' || square[5] != 'O' || square[8] != 'O')
        {
            p2Win = -1;
        } else {
            p2Win = 1;
        }  
    } else if (square[3] == square[6] && square[6] == square[9]) {
        if(square[3] != 'O' || square[6] != 'O' || square[9] != 'O')
        {
            p2Win = -1;
        } else {
            p2Win = 1;
        }
    } else if (square[1] == square[5] && square[5] == square[9]) {
        if(square[1] != 'O' || square[5] != 'O' || square[9] != 'O')
        {
            p2Win = -1;
        } else {
            p2Win = 1;
        }
        
    } else if (square[3] == square[5] && square[5] == square[7]) {
        if(square[3] != 'O' || square[5] != 'O' || square[7] != 'O')
        {
            p2Win = -1;
        } else {
            p2Win = 1;
        }
    }
}