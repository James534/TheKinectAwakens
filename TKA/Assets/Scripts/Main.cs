using UnityEngine;
using System.Collections;

public class Main : MonoBehaviour {

    Boss b;

	// Use this for initialization
	void Start () {
        GameObject temp = GameObject.Find("BoulderfistOgre");
        b = temp.GetComponent<Boss>();
	}
	
	// Update is called once per frame
	void Update () {
	    if (Input.GetKeyDown(KeyCode.A))
        {
            b.Attack();
        }
	}
}
