using UnityEngine;
using System.Collections;

public class Saber : MonoBehaviour
{
    ListenClient lc;
    Quaternion upInv;
    // Use this for initialization
    void Start()
    {
        lc = GetComponent<ListenClient>();
        upInv = Quaternion.Euler(-1, 0, -1);
    }

    // Update is called once per frame
    void Update()
    {
        string[] pos = ListenClient.ori.Split(' ');
        if (pos.Length >= 4)
        {
            Quaternion q = new Quaternion(float.Parse(pos[1]), float.Parse(pos[2]), float.Parse(pos[3]), float.Parse(pos[0]));
            //q = upInv * q;
            //q = Quaternion.Inverse(q);
            Vector3 e = q.eulerAngles;
            e.x *= -1;
            e.y *= -1;
            transform.rotation = Quaternion.Euler(e);
            //Debug.Log("transformed");
        }
    }
}
