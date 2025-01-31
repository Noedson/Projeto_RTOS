import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns


# men_means, men_std = (163.81, 177.38), (87.51, 8.35)
# women_means, women_std = (99.12, 110.81), (22.70, 13.30)

# ind = np.arange(len(men_means))  # the x locations for the groups
# width = 0.25  # the width of the bars

fig, ax = plt.subplots()
# rects1 = ax.bar(ind - width/2, men_means, width, yerr=men_std,
#                 label='with RTOS',color='paleturquoise', hatch="//")
# rects2 = ax.bar(ind + width/2, women_means, width, yerr=women_std,
#                 label='without RTOS',color='navajowhite', hatch="x")

# Add some text for labels, title and custom x-axis tick labels, etc.
# ax.set_ylabel('Data throughput (bits per second)')
# ax.set_title('95% confidence interval')
# ax.set_xticks(ind)
# ax.set_xticklabels(('RX', 'TX'))
# ax.legend(loc='upper right', ncols=2)
# ax.autoscale_view()


def autolabel(rects, xpos='center'):
    """
    Attach a text label above each bar in *rects*, displaying its height.

    *xpos* indicates which side to place the text w.r.t. the center of
    the bar. It can be one of the following {'center', 'right', 'left'}.
    """

    ha = {'center': 'center', 'right': 'left', 'left': 'right'}
    offset = {'center': 0, 'right': 1, 'left': -1}

    for rect in rects:
        height = rect.get_height()
        ax.annotate('{}'.format(height),
                    xy=(rect.get_x() + rect.get_width() / 2, height),
                    xytext=(offset[xpos]*3, 3),  # use 3 points offset
                    textcoords="offset points",  # in both directions
                    ha=ha[xpos], va='bottom')


# autolabel(rects1, "left")
# autolabel(rects2, "right")

# fig.tight_layout()
# ax.autoscale_view()

# plt.savefig(f'RTOS_Vazao.jpg', format='jpg')
# plt.savefig(f'RTOS_Vazao.eps', format='eps')
# plt.savefig(f'RTOS_Vazao.pdf', format='pdf')
# plt.savefig(f'RTOS_Vazao.png', format='png')

# plt.show()


