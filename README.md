# RTOS-Applications
## IoT Application
* MQTT - Device Roles
    * Subscriber/Publisher - MqttFx(Subscriber), My-App(Publisher)
    * Broker/Server - Openly available broker(URL - TAGs - app/feature1, app/feature2, info/device)

## Add-ons
* Threads
* Mutex, Semaphore
* Message Queue

# Thread
* Thread-1 Send/Publish 
* Thread2- Subscribe/Listen message

## Semaphore - Mutex
* HB - 100 ms
* Topic2 to send messages b/n subscriber(Consumer) and publisher(Producer)
* publisher - generates data 
* Buffer - same for Storing messages from Subscriber and 
    * Mutex
* Message queue
    * Send data to Thread3, 4 from Subscriber
    * Thread-3 - Data/sensor1 - message Q1
    * Thread-4 - Data/sensor2 - message Q2