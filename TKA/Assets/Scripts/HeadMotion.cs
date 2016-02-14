using UnityEngine;
using System.Collections;

public class HeadMotion : MonoBehaviour
{

    // Use this for initialization
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        if (ListenClient.pos != null && ListenClient.pos.Length > 5)
        {
            string[] strPos = ListenClient.pos.Split(';');
            for (int i = 0; i < strPos.Length; i++)
            {
                if (strPos[i].Contains("head"))
                {
                    string[] headPos = strPos[i].Split(' ');
                    Debug.Log("POS: " + ListenClient.pos);
                    Debug.Log("i: " + strPos[i]);
                    transform.position =
                        new Vector3(float.Parse(headPos[1]) * 5, float.Parse(headPos[2]) * 5, 7-float.Parse(headPos[3])*2);
                    break;
                }
            }
        }
    }
}