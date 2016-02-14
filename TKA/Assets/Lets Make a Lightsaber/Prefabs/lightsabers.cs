using UnityEngine;
using System.Collections;

public class lightsabers : MonoBehaviour {

	LineRenderer lineRend;
	public Transform startPos;
	public Transform endPos;
	private bool on = true;
	private Vector3 endPositionExtended;

	private float textureOffset = 0f;
	// Use this for initialization
	void Start () {
		lineRend = GetComponent<LineRenderer>();
		endPositionExtended = endPos.localPosition;

	}

	// Update is called once per frame
	void Update () {
		//update line positions
		if (Input.GetKeyDown(KeyCode.Space)) {
			if (on) {
				on = false;
			} else {
				on = true;
			}
		}

		if (on) {
			endPos.localPosition = Vector3.Lerp (endPos.localPosition, endPositionExtended, Time.deltaTime * 5f);
		} else {
			endPos.localPosition = Vector3.Lerp (endPos.localPosition, startPos.localPosition, Time.deltaTime*5f);
		}


		lineRend.SetPosition(0, startPos.position);
		lineRend.SetPosition(1, endPos.position);

		textureOffset -= Time.deltaTime * 2f;

		if (textureOffset < -10f) {
			textureOffset += 10f;

		}
		lineRend.sharedMaterials [1].SetTextureOffset ("_MainTex", new Vector2(textureOffset, 0f));
	}
}