"""
s0->s1->s2
p0,p1
pj = u/(u+(n-j)l)
p_j = ((n-j)*l)/(u+(n-j)*l)

E[w_n-i] = (1-p_i)/(l*(n-i))
"""
from scipy.interpolate import make_interp_spline
import numpy as np
import math
import matplotlib
from matplotlib import pyplot as plt
from numpy.linalg import matrix_power

def multiply_with_P(P, new_P):
    return np.dot(new_P,P)

# alpha is avg waiting time in a state
def failure_probability_calculator(n, l, u, delta_t, alpha):
    P = []
    for i in range(0,n+1,1):
        row_probab = []
        for j in range(0,n+1,1):
            if j == 0:
                row_probab.append(1-(u/(u+l*(n-i))))
            elif j == i+1:
                row_probab.append(u/(u+l*(n-i)))
            else:
                row_probab.append(0)
        P.append(row_probab)
    
    # new_P = P
    # for i in range(0,n+1,1):
    #     for j in range(0,n+1,1):
    #         for k in range(0,n+1,1):
    #             new_P[i][j]=P[i][k]*P

    P_k = P
    k=int(delta_t/alpha)
    if delta_t%alpha != 0:
        k+=1
    
    for i in range(0,k-1,1):
        P_k = multiply_with_P(P, P_k)
    
    return P_k[0][n]

def probability_matrix_calculator(n, l, u):
    P = []
    size = 0
    t_0 = 0
    for i in range(0,n,1):
        p_i = ((n-i)*l)/(u+(n-i)*l)
        t_i = math.ceil((1-p_i)/(l*(n-i)))
        # if delta_t == 20:
        #     print(p_i)
        #     print(t_i)
        # if p_i<0 or p_i>1 or t_i<1:
        #     print('error')
        if i == 0:
            t_0 = t_i
        size += t_i
    size+=1
    next_special_node = t_0 - 1
    actual_node_number = 0

    for i in range(0,size-1,1):
        row_probab = []
        for j in range(0,size,1):
            if i != next_special_node:
                if j == i+1:
                    row_probab.append(1)
                else:
                    row_probab.append(0)
            else:
                p_i = 1-(u/(u+l*(n-actual_node_number)))
                # if p_i<0 or p_i>1:
                #     print('error')
                if actual_node_number == 0:
                    if j == i+1:
                        row_probab.append(1)
                    else:
                        row_probab.append(0)
                else:
                    if j == 0:
                        row_probab.append(1-p_i)
                    elif j == i+1:
                        row_probab.append(p_i)
                    else:
                        row_probab.append(0)
        
        if i == next_special_node:
            actual_node_number+=1
            p_i_plus_1 = 1-(u/(u+l*(n-actual_node_number)))
            if actual_node_number != n:
                t_i_plus_1 = math.ceil((1-p_i_plus_1)/(l*(n-actual_node_number)))
                # if p_i_plus_1<0 or p_i_plus_1>1 or t_i_plus_1<1:
                #     print('error')
                next_special_node+=t_i_plus_1

        
        P.append(row_probab)

    row_probab = []
    for i in range(0,size-1,1):
        row_probab.append(0)
    row_probab.append(1)
    P.append(row_probab)
    # print(size)
    return P

def failure_probability_calculator_generalised(n, l, u, delta_t, P):
    # P = []
    # size = 0
    # t_0 = 0
    # for i in range(0,n,1):
    #     p_i = ((n-i)*l)/(u+(n-i)*l)
    #     t_i = math.ceil((1-p_i)/(l*(n-i)))
    #     if delta_t == 20:
    #         print(p_i)
    #         print(t_i)
    #     # if p_i<0 or p_i>1 or t_i<1:
    #     #     print('error')
    #     if i == 0:
    #         t_0 = t_i
    #     size += t_i
    # size+=1
    # next_special_node = t_0 - 1
    # actual_node_number = 0

    # for i in range(0,size-1,1):
    #     row_probab = []
    #     for j in range(0,size,1):
    #         if i != next_special_node:
    #             if j == i+1:
    #                 row_probab.append(1)
    #             else:
    #                 row_probab.append(0)
    #         else:
    #             p_i = 1-(u/(u+l*(n-actual_node_number)))
    #             # if p_i<0 or p_i>1:
    #             #     print('error')
    #             if actual_node_number == 0:
    #                 if j == i+1:
    #                     row_probab.append(1)
    #                 else:
    #                     row_probab.append(0)
    #             else:
    #                 if j == 0:
    #                     row_probab.append(1-p_i)
    #                 elif j == i+1:
    #                     row_probab.append(p_i)
    #                 else:
    #                     row_probab.append(0)
        
    #     if i == next_special_node:
    #         actual_node_number+=1
    #         p_i_plus_1 = 1-(u/(u+l*(n-actual_node_number)))
    #         if actual_node_number != n:
    #             t_i_plus_1 = math.ceil((1-p_i_plus_1)/(l*(n-actual_node_number)))
    #             # if p_i_plus_1<0 or p_i_plus_1>1 or t_i_plus_1<1:
    #             #     print('error')
    #             next_special_node+=t_i_plus_1

        
    #     P.append(row_probab)

    # row_probab = []
    # for i in range(0,size-1,1):
    #     row_probab.append(0)
    # row_probab.append(1)
    # P.append(row_probab)
    size = 0
    for i in range(0,n,1):
        p_i = ((n-i)*l)/(u+(n-i)*l)
        t_i = math.ceil((1-p_i)/(l*(n-i)))
        # if delta_t == 20:
        #     print(p_i)
        #     print(t_i)
        # if p_i<0 or p_i>1 or t_i<1:
        #     print('error')
        size += t_i
    size+=1
    k = delta_t
    # P_k = P
    # for i in range(0,k-1,1):
    #     P_k = multiply_with_P(P, P_k)

    P_k = matrix_power(P,k)
    
    return P_k[0][size-1]

# print(failure_probability_calculator_generalised(4,0.01,0.5,1000))
N = 2000
dt = 1
time_stamps = np.arange(0, N, 1)
failure_probab = []
n = 4
l = 0.0125
u = 0.1
P = probability_matrix_calculator(n,l,u)
print("done")
for i in range(N):
    failure_probab.append(failure_probability_calculator_generalised(n,l,u,time_stamps[i],P))
# failure_probab = failure_probability_calculator_generalised(4,0.1,0.5,time_stamps)
# for i in range(1000):
#     print(failure_probability_calculator_generalised(4,0.1,0.5,dt))
#     dt+=1
matplotlib.rc('xtick', labelsize=20) 
matplotlib.rc('ytick', labelsize=20)
plt.figure(figsize=(20,10))
#plt.plot(time_stamps, failure_probab, label='failure probability distribution')
plt.legend(fontsize="20")
plt.xlabel('time', fontsize="20")
plt.ylabel('Failiure Probability', fontsize="20")

for i in range(N):
    print('t = '+str(i)+' p = '+ str(failure_probab[i]))
    
pdf_f_p = []
Skip=100
j=0
for i in range(Skip,len(failure_probab),Skip):
    pdf_f_p.insert(j,failure_probab[i]-failure_probab[i-Skip])
    j=j+1
#pdf_f_p.insert(j+1,failure_probab[len(failure_probab)-1])
s = len(pdf_f_p)
s_a = []
for i in range(1,s+1):
    s_a.append(i*Skip)
#plt.scatter(s_a,pdf_f_p, label='failure probability distributionlll')
X_Y_Spline = make_interp_spline(s_a,pdf_f_p)

X_ = np.linspace(min(s_a), max(s_a), 500)
Y_ = X_Y_Spline(X_)

plt.plot(X_, Y_)







plt.savefig('s3.png')