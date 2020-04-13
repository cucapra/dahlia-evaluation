import seaborn as sns
import matplotlib
from matplotlib import pyplot as plt

print("Setting matplot configurations")
matplotlib.rcParams['pdf.fonttype'] = 42
matplotlib.rcParams['ps.fonttype'] = 42
matplotlib.rcParams['text.usetex'] = True

def make_absolute_plots(df,
                        x_key,
                        y_keys,
                        group_by,
                        x_label,
                        fig_prefix,
                        x_supress=None,
                        dpi=100,
                        fig_dir="./",
                        group_labels=None,
                        group_markers=None,
                        legend=None,
                        group_runtime_supress=None):
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

    for y_idx, key in enumerate(y_keys):
        fig = plt.figure()

        for idx, group in groups:
            if key == 'runtime_avg' and \
               group_runtime_supress and group_runtime_supress[
                    idx]:
                continue
            plt.scatter(x=x_key,
                        y=key,
                        data=group,
                        color=pal[idx],
                        label=group_labels[idx] if group_labels else None,
                        marker=group_markers[idx] if group_markers else None,
                        s=100,
                        rasterized=True)
            plt.vlines(x=x_key,
                       ymax=key,
                       ymin=df[key].min(),
                       data=group,
                       colors=pal[idx],
                       rasterized=True)

        if key == 'runtime_avg':
            y_label = 'Runtime (ms)'
        else:
            labels = key.split('_')
            labels[0] = labels[0].upper() + "s"
            y_label = " ".join(labels)

        plt.ylabel(y_label, fontsize=18)
        if x_supress is None or not x_supress[y_idx]:
            print('add xlabel')
            plt.xlabel(x_label, fontsize=18)
        plt.xticks(fontsize=16)
        plt.yticks(fontsize=16)

        ax = fig.gca()
        ax.get_yaxis().set_major_formatter(
            matplotlib.ticker.FuncFormatter(lambda x, p: format(int(x), ',')))

        if group_labels and legend == key:
            plt.legend(prop={'size': 16})

        fig.tight_layout()
        fig.savefig(fig_dir +
                    '{}-{}.pdf'.format(fig_prefix, key.replace('_', '-')),
                    dpi=dpi)

        figs.append(fig)

    return figs


def make_qual_plot(df,
                   x_data,
                   y_data,
                   group_by,
                   fig_prefix,
                   legend,
                   dpi=100,
                   fig_dir="./",
                   scale_x={
                       "scale_fun": lambda x: x,
                       "scale_label": ""
                   }):
    """
    x_data and y_data should be an objects with the key and the label names.
    """
    sns.set()
    groups = df.groupby(group_by, axis=0)
    # Mapping from group_name to color
    colors = {}
    cur = 0
    for idx, _ in groups:
        colors[idx] = cur
        cur += 1

    # Configuration for subplot
    pal = sns.color_palette('cubehelix_r', len(groups))
    sns.set_context('paper')

    fig = plt.figure()
    ax = fig.gca()

    x_key, y_key = x_data["key"], y_data["key"]
    for idx, group in groups:
        sns.scatterplot(x=x_key,
                        y=y_key,
                        data=group,
                        color=pal[colors[idx]],
                        label=idx,
                        s=100,
                        rasterized=True)

    # Format Y-axis to have commas in the numbers
    ax.get_yaxis().set_major_formatter(
        matplotlib.ticker.FuncFormatter(lambda x, p: format(int(x), ',')))

    # Scale the X-axis as needed.
    ax.get_xaxis().set_major_formatter(
        matplotlib.ticker.FuncFormatter(
            lambda x, p: format(scale_x["scale_fun"](x), ',')))

    plt.ylabel(y_data["label"])

    # Name the legend
    ax.legend(title=legend, fontsize=13)

    # Generate label for X-axis
    suf = scale_x["scale_label"]
    plt.xlabel(x_data["label"] + (" (" + suf + ")" if suf != "" else ""))

    fig.tight_layout()
    fig.savefig(fig_dir +
                '{}-{}.pdf'.format(fig_prefix, y_key.replace('_', '-')),
                dpi=dpi)

    # figs.append(fig)


def make_sec2_plot(df,
                   x_key,
                   x_label,
                   fig_prefix,
                   factor=16,
                   dpi=100,
                   fig_dir="./",
                   legend=None):
    y_keys = [
        'runtime_avg',
        'lut_used',
    ]

    def group(idx):
        row = df.iloc[idx]
        if row.status == 'error':
            return 2
        return int(factor % df.iloc[idx][x_key] == 0)

    make_absolute_plots(df,
                        x_key,
                        y_keys,
                        dpi=dpi,
                        fig_dir=fig_dir,
                        group_by=group,
                        group_labels=[
                            'Unpredictable points', 'Predictable points',
                            'Incorrect hardware'
                        ],
                        group_markers=['o', 'v', 'x'],
                        group_runtime_supress=[False, False, True],
                        x_supress=[False, True],
                        x_label=x_label,
                        fig_prefix=fig_prefix,
                        legend=legend)
