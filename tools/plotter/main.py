import matplotlib.pyplot as plt; plt.rcdefaults()
import matplotlib.pyplot as plt
import numpy as np
import json

def generate_throughput_graph_for(json_file_name, test_file, plot_name="speed.png"):
    with open(json_file_name) as f:
        data = json.load(f)
    libs = []
    thrghput = []
    for e in data:
        if e['filename'] == test_file:
            thrghput.append(e['throughput'])
            libs.append(e['benchname'])

    y_pos = np.arange(len(libs))

    plt.bar(y_pos, thrghput, align='center', alpha=0.5)
    plt.xticks(y_pos, libs, rotation=45)
    plt.ylabel('Speed in Gb/s')
    plt.title(f"speed of parsing {test_file} in Gb per sec")
    fig = plt.gcf()
    fig.set_size_inches(18.5, 12.5)
    

    plt.savefig(plot_name, dpi=400)

generate_throughput_graph_for("out.json", "data_simdjson/gsoc-2018.json")