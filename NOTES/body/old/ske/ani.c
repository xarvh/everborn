



typedef struct skeAniSetS {
    skeAnimationT *ani;
    unsigned ani_cnt;
} skeAniSetT;


#define SKE_STAND	0
#define SKE_IDLE	1
#define SKE_WALK	2
#define SKE_RUN		3
#define SKE_MELEE	4
#define SKE_RANGE	5
#define SKE_DIE		6
#define SKE_CRY		7
#define SKE_TOTAL	8

typedef struct skeAnimatorS {
    skeAniSetT	set[SKE_TOTAL];
} skeAnimatorT;



skeAnimationT *animator(animatorT *a, int animation_id)
{
 skeAniSetT	*set;

 set = a->set + animation_id;
 return set->ani + (random() % set->ani_cnt);
}






//EOF
