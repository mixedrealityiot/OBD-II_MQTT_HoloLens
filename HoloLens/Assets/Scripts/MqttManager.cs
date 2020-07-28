using Newtonsoft.Json;
using System;
using System.Net.Sockets;
using TMPro;
using UnityEngine;
using uPLibrary.Networking.M2Mqtt;
using uPLibrary.Networking.M2Mqtt.Messages;

public class MqttManager : MonoBehaviour
{
    #region Properties
    /// <summary>
    /// Mqtt properties
    /// </summary>
    private MqttClient client;
    private string broker;
    private int port;
    private bool secure;
    private MqttSslProtocols sslprotocol;
    private MqttProtocolVersion protocolversion;
    private string username;
    private string password;
    private string clientId;
    private string topic;
    private bool publish = false;
    private byte qos;
    private bool retain;
    private bool cleansession;
    private ushort keepalive;

    /// <summary>
    /// Data properties
    /// </summary>
    private bool dataHasBeenUpdated;
    private Telemetry data;

    /// <summary>
    /// Text objects assoicated with the Mqtt data
    /// </summary>
    public TextMeshProUGUI TemperatureText;
    public TextMeshProUGUI RPMText;

    /// <summary>
    /// Setting UI
    /// </summary>
    public SettingMenu settings;

    #endregion

    #region Initilization
    /// <summary>
    /// Start is called before the first frame update.
    /// </summary>
    private void Start()
    {
        //Set up default Mqtt properties for https://github.com/khilscher/MqttClient
        broker = "mqtt.eclipse.org";
        port = 1883;
        secure = false;
        sslprotocol = MqttSslProtocols.None;
        protocolversion = MqttProtocolVersion.Version_3_1_1;
        username = "";
        password = "";
        clientId = "client123abcd";
        topic = "vehicle/telemetry";
        publish = false;
        qos = (byte)2;
        retain = false;
        cleansession = false;
        keepalive = 60;

        //Update the settings menu with default values
        settings.MQTTBrokerTextInputButton.TextObject.text = broker;
        settings.DeviceIDTextInputButton.TextObject.text = clientId;
        settings.TopicTextInputButton.TextObject.text = topic;

        //Default data properties
        data = null;
        dataHasBeenUpdated = false;

        //Add callback methods for setting ui
        settings.ConnectButton.OnClick.AddListener(() => MqttConnect());
        settings.DisconnectButton.OnClick.AddListener(() => MqttDisconnect());
        settings.SubscribeButton.OnClick.AddListener(() => MqttSubscribe());
        settings.UnsubscribeButton.OnClick.AddListener(() => MqttUnsubscribe());
    }
    #endregion

    #region Methods

    /// <summary>
    /// Update is called every frame, if the MonoBehaviour is enabled.
    /// </summary>
    private void Update()
    {
        //if we have new data, update the corresponding text in the scene
        if (dataHasBeenUpdated)
        {
            TemperatureText.text = data.engineTemp.ToString();
            RPMText.text = data.engineRpm.ToString();
            dataHasBeenUpdated = false;
        }
    }

    /// <summary>
    /// Updated MQTT based on values in the setting menu.
    /// </summary>
    public void UpdateMqttBasedOnSettingMenu()
    {
        broker = settings.MQTTBrokerTextInputButton.TextObject.text;
        clientId = settings.DeviceIDTextInputButton.TextObject.text;
        topic = settings.TopicTextInputButton.TextObject.text;
    }

    /// <summary>
    /// Connected to Mqtt.
    /// </summary>
    public void MqttConnect()
    {
        try
        {
            UpdateMqttBasedOnSettingMenu();
            client = new MqttClient(this.broker);

            // Set MQTT version
            client.ProtocolVersion = this.protocolversion;

            // Setup callback for receiving messages
            client.MqttMsgPublishReceived += ClientRecieveMessage;

            // MQTT return codes 
            // https://www.hivemq.com/blog/mqtt-essentials-part-3-client-broker-connection-establishment/
            // https://www.eclipse.org/paho/clients/dotnet/api/html/4158a883-de72-1ec4-2209-632a86aebd74.htm
            byte resp = client.Connect(this.clientId, this.username, this.password, this.cleansession, this.keepalive);
            settings.DebugConsole.text = "Connect() Response: " + resp.ToString();
        }
        catch (SocketException e)
        {
            settings.DebugConsole.text = e.Message;
        }
        catch (Exception e)
        {
            settings.DebugConsole.text = e.Message;

        }
    }

    /// <summary>
    /// Subscribe to predefined Mqtt topic.
    /// </summary>
    public void MqttSubscribe()
    {
        if (client != null && client.IsConnected)
        {
            ushort resp = client.Subscribe(
                new string[] { this.topic },
                new byte[] { this.qos });

            settings.DebugConsole.text = "Subscribe() Response: " + resp.ToString();

        }
        else
        {
            settings.DebugConsole.text = "Not subscribe";

        }
    }

    /// <summary>
    /// Call back Method for recieving messages from MQTT.
    /// </summary>
    public void ClientRecieveMessage(object sender, MqttMsgPublishEventArgs e)
    {
        data = JsonConvert.DeserializeObject<Telemetry>(System.Text.UTF8Encoding.UTF8.GetString(e.Message));
        dataHasBeenUpdated = true;
    }

    /// <summary>
    /// Disconnect from Mqtt broker.
    /// </summary>
    private void MqttDisconnect()
    {
        if (client != null && client.IsConnected)
        {
            client.Disconnect();
            settings.DebugConsole.text = "Disconnect()";
        }
        else
        {
            settings.DebugConsole.text = "Not connected";
        }
    }

    /// <summary>
    /// Unsubscribe from Mqtt topic.
    /// </summary>
    private void MqttUnsubscribe()
    {
        if (client != null && client.IsConnected)
        {
            ushort resp = client.Unsubscribe(
                new string[] { this.topic });

            settings.DebugConsole.text = "Unsubscribe() Response: " + resp.ToString();
        }
        else
        {
            settings.DebugConsole.text = "Not connected";

        }
    }
    #endregion
}
