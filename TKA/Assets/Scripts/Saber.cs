using UnityEngine;
using System.Collections;

public class Saber : MonoBehaviour
{
    ListenClient lc;
    Quaternion upInv;
    GameObject pivot;
    // Use this for initialization
    void Start()
    {
        lc = GetComponent<ListenClient>();
        upInv = Quaternion.Euler(-1, 0, -1);
        pivot = GameObject.Find("Lightsaber");
    }

    // Update is called once per frame
    void Update()
    {
        if (ListenClient.ori != null && ListenClient.ori.Length > 4)
        {
            string[] difPos = ListenClient.ori.Split('_');
            string[] pos = difPos[0].Split(' ');
            double accel = double.Parse(difPos[1]);
            int button = int.Parse(difPos[2]);
            int trigger = int.Parse(difPos[3]);
            byte[] bytes = System.BitConverter.GetBytes(button);
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
