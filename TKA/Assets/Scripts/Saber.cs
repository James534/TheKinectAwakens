using UnityEngine;
using System.Collections;

public class Saber : MonoBehaviour
{
    ListenClient lc;
    Quaternion upInv;
    GameObject pivot;
	public AudioClip impact;
	public AudioSource audio;
	public static bool globalExtended = false;
	public BitArray b;
	int endPlay = 2;
    // Use this for initialization
    void Start()
    {
        lc = GetComponent<ListenClient>();
        upInv = Quaternion.Euler(-1, 0, -1);
        pivot = GameObject.Find("Lightsaber");
		var audios = GetComponents<AudioSource>();
		audio = audios [1];
		audio.timeSamples = 100;
    }

    // Update is called once per frame
    void Update()
    {
        if (ListenClient.ori != null && ListenClient.ori.Length > 4)
        {
			//Debug.Log (ListenClient.ori);
            string[] difPos = ListenClient.ori.Split('_');
			Debug.Log (difPos);
            string[] pos = difPos[0].Split(' ');

            double accel = double.Parse(difPos[1]);
			if (accel >= 7000 && !audio.isPlaying) {
				audio.Play ();
			}
            int button = int.Parse(difPos[2]);
            int trigger = int.Parse(difPos[3]);
			Debug.Log (button);
			b = new BitArray (new int[] { button });
			bool[] bits = new bool[32];
			if (button != 0) {
				b.CopyTo (bits, 0);

			}
			Debug.Log ("button " + bits[19]);
			if (bits [19]) {
				globalExtended = true;
			} else {
				globalExtended = false;
			}

            if (pos.Length >= 4)
            {
                Quaternion q = new Quaternion(float.Parse(pos[1]), float.Parse(pos[2]), float.Parse(pos[3]), float.Parse(pos[0]));
                //q = upInv * q;
                //q = Quaternion.Inverse(q);
                Vector3 e = q.eulerAngles;
                e.x *= -1;
                e.y *= -1;
                pivot.transform.rotation = Quaternion.Euler(e);
                //Debug.Log("transformed");
            }

        }

        if (ListenClient.pos != null && ListenClient.pos.Length > 5)
        {
            string[] strPos = ListenClient.pos.Split(';');
            for (int i = 0; i < strPos.Length; i++)
            {
                if (strPos[i].Contains("rwrist"))
                {
                    string[] headPos = strPos[i].Split(' ');
                    Debug.Log("POS: " + ListenClient.pos);
                    Debug.Log("i: " + strPos[i]);
                    transform.position =
                        new Vector3(float.Parse(headPos[1]) * 5, float.Parse(headPos[2]) * 5, 7 - float.Parse(headPos[3]) * 2);
                    break;
                }
            }
        }
    }

    void OnTriggerEnter(Collider other)
    {
        if (other.gameObject.name.Equals("BoulderfistOgre"))
        {
            Boss b = other.gameObject.GetComponent<Boss>();
            b.Damage(10);
        }
    }

    void OnParticleCollision(GameObject other)
    {
        Debug.Log("Hit");
        //GameObject.Destroy(other);
    }
}

