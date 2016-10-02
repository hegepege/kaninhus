package no.vs11.rabbit;

import org.eclipse.paho.client.mqttv3.*;

public class MqttRabbitReceiver implements MqttCallback{

    MqttClient myClient;
    MqttConnectOptions connOpt;

    static final String BROKER_URL = "tcp://trygvis.io:1883";
    static final String M2MIO_DOMAIN = "vs11";
    static final String M2MIO_STUFF = "kanin";
    static final String M2MIO_THING = "temp";

    // the following two flags control whether this example is a publisher, a subscriber or both
    static final Boolean subscriber = true;

    /**
     * connectionLost
     * This callback is invoked upon losing the MQTT connection.
     */
    public void connectionLost(Throwable t) {
        System.out.println("Connection lost!");
        connectToBroker();
    }




    /**
     * deliveryComplete
     * This callback is invoked when a message published by this client
     * is successfully received by the broker.
     */
    public void deliveryComplete(IMqttDeliveryToken token) {
        //System.out.println("Pub complete" + new String(token.getMessage().getPayload()));
    }

    /**
     * messageArrived
     * This callback is invoked when a message is received on a subscribed topic.
     */
    public void messageArrived(String topic, MqttMessage message) throws Exception {
        System.out.println("-------------------------------------------------");
        System.out.println("| Topic:" + topic);
        System.out.println("| Message: " + new String(message.getPayload()));
        System.out.println("-------------------------------------------------");
    }

    /**
     * MAIN
     */
    public static void main(String[] args) {
        MqttRabbitReceiver smc = new MqttRabbitReceiver();
        smc.runClient();
    }

    /**
     * runClient
     * The main functionality of this simple example.
     * Create a MQTT client, connect to broker, pub/sub, disconnect.
     */
    public void runClient() {
        connectToBroker();
        System.out.println("Connected to " + BROKER_URL);

        // setup topic
        // topics on m2m.io are in the form <domain>/<stuff>/<thing>
        String myTopic = M2MIO_DOMAIN + "/" + M2MIO_STUFF + "/" + M2MIO_THING;
        System.out.println("Topic: " + myTopic);
        MqttTopic topic = myClient.getTopic(myTopic);

        // subscribe to topic if subscriber
        if (subscriber) {
            try {
                int subQoS = 0;
                myClient.subscribe(myTopic, subQoS);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        // disconnect
        try {
            // wait to ensure subscribed messages are delivered
            if (subscriber) {
                Thread.sleep(30000);
            }
            System.out.println("Disconnecting");
            myClient.disconnect();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void connectToBroker() {
        // setup MQTT Client
        String clientID = M2MIO_THING;
        connOpt = new MqttConnectOptions();

        connOpt.setCleanSession(true);
        connOpt.setKeepAliveInterval(30);

        // Connect to Broker
        try {
            myClient = new MqttClient(BROKER_URL, clientID);
            myClient.setCallback(this);
            myClient.connect(connOpt);
        } catch (MqttException e) {
            e.printStackTrace();
            System.exit(-1);
        }
    }
}
