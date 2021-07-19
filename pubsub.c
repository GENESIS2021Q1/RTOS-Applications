#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <time.h>
#include <unistd.h>
#include "MQTTClient.h"
#include<pthread.h>
#include<semaphore.h>

#define ADDRESS     "tcp://broker.hivemq.com:1883"
#define CLIENTID    "Bharath-pub-sub"
#define TOPIC       "Demo"
#define PAYLOAD     "Device - HB : "
#define QOS         1
#define TIMEOUT     10000L
volatile MQTTClient_deliveryToken deliveredtoken;

MQTTClient client;
sem_t s1;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    char* payloadptr;
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: ");
    payloadptr = message->payload;
    for(i=0; i<message->payloadlen; i++)
    {
        putchar(*payloadptr++);
    }
    putchar('\n');
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}
void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

void* publish_task(void* args)
{
    sem_wait(&s1);
    int rc;
        char Data[100] = PAYLOAD;
    time_t rawtime;
    struct tm *timeinfo;
    MQTTClient_deliveryToken token;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
   
    for (int i = 0; i < 10; i++)
    {
        time(&rawtime);
        timeinfo = localtime(&rawtime);

        memset(Data, 0, 100);
        strcat(Data, PAYLOAD);
        strcat(Data, asctime(timeinfo));

        //    pubmsg.payload = PAYLOAD;
        //    pubmsg.payloadlen = strlen(PAYLOAD);
        pubmsg.payload = Data;
        pubmsg.payloadlen = strlen(Data);
        pubmsg.qos = QOS;
        pubmsg.retained = 0;

        MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
        printf("Waiting for up to %d seconds for publication of %s\n"
               "on topic %s for client with ClientID: %s\n",
               (int)(TIMEOUT / 1000), PAYLOAD, TOPIC, CLIENTID);
        rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);

        printf("Message with delivery token %d delivered\n", token);
        sleep(1);
    }

}
void* subscribe_task(void* args)
{
    int ch;
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
    MQTTClient_subscribe(client, TOPIC, QOS);
    sem_post(&s1);
    do
    {
        sleep(1);
    } while(1);
}

int main(int argc, char* argv[])
{
    
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

    void* exit_status_sub = NULL;
    void* exit_status_pub = NULL;
	pthread_t pub_pt, sub_pt;

    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
   	sem_init(&s1,0,0);
   // sem_init(&s2,0,1);
	pthread_create(&pub_pt,NULL,publish_task,"Thread");
    pthread_create(&sub_pt,NULL,subscribe_task,"Thread");

    
    pthread_join(pub_pt,&exit_status_pub);
    pthread_join(sub_pt,&exit_status_sub);
    
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}

