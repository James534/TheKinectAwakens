using UnityEngine;
using System.Collections;

public class Boss : MonoBehaviour
{

    Animator anim;
    float[] animLength = new float[4];  //0=idle, 1=death, 2=attack, 3=move
    double runTime = 0;
    int curAnim = 0;
    public bool dead = false;
    public GameObject aoe;

    bool wait = false;
    double startTime = 0;
    bool playedParticle = false;

    public float maxHealth = 100;
    public float curHealth = 100;

    // Use this for initialization
    void Start()
    {
        anim = GetComponent<Animator>();
        RuntimeAnimatorController ac = anim.runtimeAnimatorController;
        for (int i = 0; i < ac.animationClips.Length; i++)
        {
            if (ac.animationClips[i].name.Equals("Stand"))
            {
                animLength[0] = ac.animationClips[i].length;
            }
            else if (ac.animationClips[i].name.Equals("AttackUnarmed") ||
               ac.animationClips[i].name.Equals("Attack2HL") ||
               ac.animationClips[i].name.Equals("AttackOff"))
            {
                animLength[2] = ac.animationClips[i].length - 0.1f;
            }
            else if (ac.animationClips[i].name.Equals("Death"))
            {
                animLength[1] = ac.animationClips[i].length - 0.1f;
            }
            else if (ac.animationClips[i].name.Equals("Walk"))
            {
                animLength[3] = ac.animationClips[i].length - 0.1f;
            }
            else if (ac.animationClips[i].name.Equals("CombatWound") ||
               ac.animationClips[i].name.Equals("StandWound"))
            {
                animLength[2] = ac.animationClips[i].length;
            }
            Debug.Log(ac.animationClips[i].name + " " + ac.animationClips[i].length);
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (curAnim != 0)
        {
            if (anim.GetTime() - runTime > animLength[curAnim])
            {
                if (curAnim == 1)
                {
                    this.transform.position = new Vector3(-5000, -5000, 0);
                    dead = true;
                }

                anim.Play("Idle");
            }
            if (!playedParticle && curAnim == 2 && anim.GetTime() - runTime > 0.6)
            {
                playedParticle = true;
                GameObject temp = Instantiate(aoe);
                Destroy(temp, 3);
                Damage(10);
            }
        }
    }

    public void Attack()
    {
        anim.Play("Attack");
        runTime = anim.GetTime();
        curAnim = 2;
        playedParticle = false;
    }
    
    public void Damage(float dmg)
    {
        curHealth -= dmg;
        if (curHealth <= 0)
        {
            curHealth = 0;
            Death();
        }
    }

    public float getAtkTime()
    {
        return animLength[2];
    }

    public void Death()
    {
        anim.Play("Death");
        runTime = anim.GetTime();
        curAnim = 1;
    }

    public void Hit()
    {
        anim.Play("Hurt");
        runTime = anim.GetTime();
        curAnim = 2;
    }
}
