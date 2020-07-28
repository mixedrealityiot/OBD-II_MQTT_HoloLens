using Microsoft.MixedReality.Toolkit.UI;
using UnityEngine;

public class SettingMenu : MonoBehaviour
{
    public TextInputObject MQTTBrokerTextInputButton;
    public TextInputObject DeviceIDTextInputButton;
    public TextInputObject TopicTextInputButton;
    public Interactable ConnectButton;
    public Interactable DisconnectButton;
    public Interactable SubscribeButton;
    public Interactable UnsubscribeButton;
    public TextMesh DebugConsole;
}
