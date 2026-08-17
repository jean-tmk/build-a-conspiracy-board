#!/usr/bin/env python3
import json, random, sys
SEEDS=[("The blue envelope","document"),("Room 314","place"),("A second shadow","signal"),("Monday's witness","person"),("The borrowed watch","object"),("Eleven missing minutes","event")]
def generate(seed=17):
    random.seed(seed); nodes=[]
    for i,(title,kind) in enumerate(SEEDS,1): nodes.append({"id":i,"title":title,"type":kind,"confidence":round(random.uniform(.42,.94),2),"x":random.randint(8,86),"y":random.randint(10,78)})
    edges=[{"source":i,"target":i+1,"label":random.choice(["same timestamp","mentioned twice","unexplained overlap","found nearby"]),"weight":round(random.uniform(.4,.9),2)} for i in range(1,len(nodes))]
    return {"title":"The Blue Envelope","nodes":nodes,"edges":edges}
if __name__=="__main__": json.dump(generate(int(sys.argv[1]) if len(sys.argv)>1 else 17),sys.stdout,indent=2)
