import pandas as pd
import seaborn as sns
from matplotlib import pyplot as plt

def make_absolute_plots(
        df,
        x_key,
        y_keys,
        group_by,
        x_label,
        fig_prefix,
        group_labels = None,
        group_markers = None,
        group_runtime_supress = None):
    """Make and save graphs plotting each value in `y_keys` against `x_key`.
    Plotting each set of points separately by first running group_by on each
    row.
    """

    sns.set()

    figs = []

    groups = df.groupby(group_by, axis=0)

    # Configuration for subplot
    pal = sns.color_palette('colorblind', len(groups))
    sns.set_context('paper')

    for key in y_keys:
        fig = plt.figure()

        for idx, group in groups:
            if key == 'runtime_avg' and group_runtime_supress and group_runtime_supress[idx]:
                continue
            plt.scatter(x=x_key, y=key, data=group, color=pal[idx],
                        label=group_labels[idx] if group_labels else None,
                        marker=group_markers[idx] if group_markers else None,
                        s=100)
            plt.vlines(x=x_key, ymax=key, ymin=df[key].min(), data=group, colors=pal[idx])

        if key == 'runtime_avg':
            y_label = 'Runtime (ms)'
        else:
            y_label = key.replace('_', ' ').capitalize()

        plt.ylabel(y_label, fontsize=18)
        plt.xlabel(x_label, fontsize=18)
        plt.xticks(fontsize=16)
        plt.yticks(fontsize=16)

        if group_labels:
            plt.legend(prop={'size': 16})

        fig.tight_layout()
        fig.savefig('{}-{}.pdf'.format(fig_prefix, key.replace('_', '-')), dpi=100)

        figs.append(fig)

    return figs

def make_sec2_plot(df, x_key, x_label, fig_prefix, factor=16):
    y_keys = [
        'runtime_avg',
        'lut_used',
    ]

    def group(idx):
        row = df.iloc[idx]
        if row.status == 'error':
            return 2
        return int(factor % df.iloc[idx][x_key] == 0)

    make_absolute_plots(
        df,
        x_key,
        y_keys,
        group_by = group,
        group_labels = ['Unpredictable points', 'Predictable points', 'Incorrect hardware'],
        group_markers = ['o', 'v', 'x'],
        group_runtime_supress = [ False, False, True ],
        x_label = x_label,
        fig_prefix = fig_prefix)
