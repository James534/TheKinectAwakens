using UnityEngine;
using System.Collections;

public class Healthbar : MonoBehaviour {

    Boss b;

	// Use this for initialization
	void Start () {
        GameObject temp = GameObject.Find("BoulderfistOgre");
        b = temp.GetComponent<Boss>();
	}
	
	// Update is called once per frame
	void Update () {
        Vector3 scale = transform.localScale;
        scale.x = b.curHealth / b.maxHealth;
        transform.localScale = scale;
	}
}
