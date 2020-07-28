using Microsoft.MixedReality.Toolkit.Experimental.UI;
using UnityEngine;

public class TextInputManager : MonoBehaviour
{
    #region Properties
    /// <summary>
    /// Access to the keyboard script.
    /// </summary>
    public MixedRealityKeyboard keyboard;

    /// <summary>
    /// All the Text Input Object in the scene. 
    /// </summary>
    public TextInputObject[] TextInputObjects;

    /// <summary>
    /// Reference to the Textbox the keyboard is currently updating.
    /// </summary>
    private TextInputObject CurrentlyActiveTextbox = null;
    #endregion

    #region Initilization
    /// <summary>
    /// Start is called on the frame when a script is enabled.
    /// </summary>
    private void Start()
    {
        //Add callback method and set the button to default state
        for (int i = 0; i < TextInputObjects.Length; i++)
        {
            int currentIndex = i;
            TextInputObjects[currentIndex].Button.CurrentDimension = 0;
            TextInputObjects[currentIndex].Button.OnClick.AddListener(() => UpdateKeyboardState(TextInputObjects[currentIndex]));
        }
    }
    #endregion

    #region Methods

    /// <summary>
    /// Update is called every frame, if the MonoBehaviour is enabled.
    /// </summary>
    private void Update()
    {
        //Update text object if users has inputed new text in the keyboard.
        if (!(CurrentlyActiveTextbox == null))
        {
            if (!CurrentlyActiveTextbox.TextObject.text.Equals(keyboard.Text))
            {
                CurrentlyActiveTextbox.TextObject.text = keyboard.Text;
            }
        }
    }

    /// <summary>
    /// Update the text button object properties and callback methods based on the text input button state.
    /// </summary>
    /// <param name="textInput"></param>
    public void UpdateKeyboardState(TextInputObject textInput)
    {
        ResetKeyboardProperties();

        //If the button is on an active state
        if (textInput.Button.CurrentDimension.Equals(1))
        {
            keyboard.OnHideKeyboard.AddListener(() => ResetKeyboardProperties());

            CurrentlyActiveTextbox = textInput;
            keyboard.ShowKeyboard();
        }
    }

    /// <summary>
    /// Reset keyboard
    /// </summary>
    public void ResetKeyboardProperties()
    {
        //Reset keyboard
        keyboard.ClearKeyboardText();
        keyboard.HideKeyboard();

        //Remove all events attached to the keyboard
        keyboard.OnHideKeyboard.RemoveAllListeners();

        //Reset currently active button
        if (!(CurrentlyActiveTextbox == null))
        {
            CurrentlyActiveTextbox.Button.CurrentDimension = 0;
        }
        CurrentlyActiveTextbox = null;
    }
    #endregion
}
