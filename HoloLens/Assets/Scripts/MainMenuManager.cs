using Microsoft.MixedReality.Toolkit.UI;
using UnityEngine;

public class MainMenuManager : MonoBehaviour
{
    #region Properties
    /// <summary>
    /// Seralized array for connect the UI button to the correct Object in scene.
    /// </summary>
    [System.Serializable]
    public class VisualizationUIEntry
    {
        public GameObject Visualization;
        public Interactable UIButton;
    }
    public VisualizationUIEntry[] VisualizationsAndUI;

    #endregion

    #region Initalization
    void Start()
    {
        InitalSetUpOfAllButtonsAndVisualizations();
        ConnectVisualizationsToUI();
    }
    #endregion

    #region Methods

    /// <summary>
    /// Init Menu settings.
    /// </summary>
    public void InitalSetUpOfAllButtonsAndVisualizations()
    {
        //reset everything to default state
        for (int i = 0; i < VisualizationsAndUI.Length; i++)
        {
            //reset buttons
            VisualizationsAndUI[i].UIButton.CurrentDimension = 0;

            //reset visulizations show or hide
            VisualizationsAndUI[i].Visualization.SetActive(false);


        }
    }

    /// <summary>
    /// Added Clicking funtionality to Menu.
    /// </summary>
    public void ConnectVisualizationsToUI()
    {
        foreach (VisualizationUIEntry entry in VisualizationsAndUI)
        {
            entry.UIButton.OnClick.AddListener(() => ToggleVisualizations(entry.Visualization));
        }
    }

    /// <summary>
    /// Method that is called when a button is clicked to toggle visualization on or off.
    /// </summary>
    public void ToggleVisualizations(GameObject Visualization)
    {
        Visualization.SetActive(!Visualization.activeSelf); 
    }
    #endregion
}
