using UnityEngine;
using System.Collections;

public class PlayerHPBar : MonoBehaviour {

    Player p;
	// Use this for initialization
	void Start () {
        GameObject temp = GameObject.Find("Main Camera");
        p = temp.GetComponent<Player>();
	}
	
	// Update is called once per frame
	void Update () {
        Vector3 scale = transform.localScale;
        scale.x = p.hp / p.maxHp;
        transform.localScale = scale;
	}
}
