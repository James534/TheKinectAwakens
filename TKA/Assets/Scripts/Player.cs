using UnityEngine;
using System.Collections;

public class Player : MonoBehaviour {


    public float hp = 100f;
    public float maxHp = 100;

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
	    if (Input.GetKeyDown(KeyCode.Q))
        {
            Damage(10);
        }
	}

    void Damage(float d)
    {
        hp -= d;
        if (hp <= 0)
        {
            hp = 0;
        }
    }
}
