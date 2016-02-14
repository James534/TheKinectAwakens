using UnityEngine;
using System.Collections;

public class lightsaber : MonoBehaviour {

	LineRenderer lineRend;
	public Transform startPos;
	public Transform endPos;

	private float textureOffset = 0f;
	// Use this for initialization
	void Start () {
		lineRend = GetComponent<LineRenderer>();
	
	}
	
	// Update is called once per frame
	void Update () {
		//update line positions
		lineRend.SetPosition(0, startPos.position);
		lineRend.SetPosition(1, endPos.position);

		textureOffset -= Time.deltaTime * 2f;

		if (textureOffset < -10f) {
			textureOffset += 10f;
	
		}
		lineRend.sharedMaterials [1].SetTextureOffset ("_MainTex", new Vector2(textureOffset, 0f));
	}
}
