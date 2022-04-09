phase_1: 
Border relations with Canada have never been better.
phase_2:
1 2 4 8 16 32
phase_3:
3 256
phase_4:
7 0
func4(a,b,c)
{
	ret=c;
	ret-=b;
	ret+=(ret>>31)//logical右移
	ret>>=1;
	ecx=ret+b;
	if(ecx<=a)
	{
		if(ecx==a)return 0;	//?????
		else{	
			func(a,a1+1,c);
			return 2*ret+1;
		}				
	}
	else
	{
		c=ecx-1;
		func(a,b,c);//func(a,b,ret+b-1);
		return ret*2;
	}
}
phase_5:
9on567("flyers" offsets:9fe567)
phase_6:
4 3 2 1 6 5  (ai = 7-ai)=>(降序排列这6个节点)
