import matplotlib.pyplot as plt
import os
import re

def parse_log_files(log_dir):
    performance_data = {}
    log_files = [f for f in os.listdir(log_dir) if os.path.isfile(os.path.join(log_dir, f))]
    data_sizes_order = [] # Keep order of data sizes for x-axis

    for log_file in sorted(log_files, key=lambda x: int(re.search(r"exp_(\d+)M\.log", x).group(1)) if re.search(r"exp_(\d+)M\.log", x) else 0): # sort by data size
        file_path = os.path.join(log_dir, log_file)
        data_size_match = re.search(r"exp_(\d+M)\.log", log_file)
        if not data_size_match:
            continue
        data_size = data_size_match.group(1)
        if data_size not in data_sizes_order:
            data_sizes_order.append(data_size)
        performance_data[data_size] = performance_data.get(data_size, {})

        with open(file_path, 'r') as f:
            content = f.readlines()

        data_structure_name = None
        construction_time = None
        insert_time = None
        query_time = None
        successor_query_time = None
        delete_time = None

        for line in content:
            if "Testing Binary Search Tree (BST) by std::set" in line:
                data_structure_name = "std::set"
            elif "Testing sequencial model by std::vector" in line:
                data_structure_name = "std::vector"
            elif "Testing van Emde Boas Tree" in line:
                data_structure_name = "vEB-tree"


            construction_match = re.search(r"Time to construction: (.*?) secs", line)
            if construction_match:
                construction_time = float(construction_match.group(1).strip())

            insert_match = re.search(r"Time to insert .*?: (.*?) secs", line)
            if insert_match:
                insert_time = float(insert_match.group(1).strip())

            query_match = re.search(r"Time to query .*?: (.*?) secs", line)
            if query_match:
                query_time = float(query_match.group(1).strip())

            successor_match = re.search(r"Time to successor query .*?: (.*?) secs", line)
            if successor_match:
                successor_query_time = float(successor_match.group(1).strip())

            remove_match = re.search(r"Time to delete .*?: (.*?) secs", line)
            if remove_match:
                delete_time = float(remove_match.group(1).strip())

            performance_data[data_size][data_structure_name] = {
                "construction": construction_time,
                "insert": insert_time,
                "query": query_time,
                "successor": successor_query_time,
                "delete": delete_time,
            }

    return performance_data, data_sizes_order

def create_line_graph(data, data_sizes_order, metric, title, ylabel, filename):
    plot_data_sizes_order = []
    if metric == "delete":
        for size in data_sizes_order:
            x = (int)(size.split('M')[0])
            y = x // 2 if x > 1 else 0.5
            new_x_key = (str)(y) + 'M/' + (str)(x) + 'M'
            plot_data_sizes_order.append(new_x_key)
    else:
        plot_data_sizes_order = data_sizes_order
    plt.figure(figsize=(6, 4))
    data_structures = set()
    for size_data in data.values():
        data_structures.update(size_data.keys())
    data_structures = sorted(list(data_structures)) # Ensure consistent order

    for structure_name in data_structures:
        times = []
        for size in data_sizes_order:
            if structure_name in data[size] and data[size][structure_name][metric] is not None:
                times.append(data[size][structure_name][metric])
            else:
                times.append(float('nan')) # Handle missing data by plotting NaN, which will create gaps

        linestyle = '-'
        if structure_name == "std::vector":
            if metric == "insert" or metric == "delete":
                continue
            linestyle = '--'
        if structure_name == "vEB-tree":
            plt.plot(plot_data_sizes_order, times, linestyle=linestyle, color="green", label=structure_name)
            continue
        plt.plot(plot_data_sizes_order, times, linestyle=linestyle, label=structure_name)

    if metric == "insert":
        times = []
        for size in data_sizes_order:
            if data[size]["vEB-tree"]["construction"]:
                times.append(data[size]["vEB-tree"]["construction"])
            else:
                times.append(float('nan'))
        plt.plot(data_sizes_order, times, linestyle='--', color='r', label='vEB-tree build')

    # plt.xlabel("Data Size (Query Size)")
    plt.xlabel("Deletion Size/Data Size")
    plt.ylabel(ylabel)
    plt.title(title)
    # plt.grid(axis='y', linestyle='--')
    plt.xticks(ha='center', rotation=45)
    plt.legend()
    plt.tight_layout()
    plt.savefig(filename + ".pdf")
    plt.close()


def create_construction_bar_graph(data, data_sizes_order, title, ylabel, filename):
    data_structures = set()
    for size_data in data.values():
        data_structures.update(size_data.keys())
    data_structures = sorted(list(data_structures))
    num_structures = len(data_structures)
    bar_width = 0.8 / num_structures  # Adjust bar width for number of structures
    group_positions = list(range(len(data_sizes_order)))

    plt.figure(figsize=(12, 7))

    # colors = ['skyblue', 'lightcoral', 'lightgreen', 'lightsalmon', 'gold', 'orchid', 'cadetblue', 'lightseagreen'] # More colors

    for i, structure_name in enumerate(data_structures):
        construction_times = []
        for size in data_sizes_order:
            if structure_name in data[size] and data[size][structure_name]['construction'] is not None:
                construction_times.append(data[size][structure_name]['construction'])
            else:
                construction_times.append(0) # Use 0 for missing construction time in bar chart

        positions = [p + i * bar_width for p in group_positions]
        plt.bar(positions, construction_times, width=bar_width, label=structure_name,)

    plt.xlabel("Data Size")
    plt.ylabel(ylabel)
    plt.title(title)
    plt.xticks([p + bar_width * (num_structures - 1) / 2 for p in group_positions], data_sizes_order, ha='center')
    # plt.legend()
    # plt.grid(axis='y', linestyle='--')
    plt.tight_layout()
    plt.savefig(filename + ".pdf")
    plt.close()


# Assuming log files are in a directory named 'logs'
log_directory = 'logs'
performance_data, data_sizes_order = parse_log_files(log_directory)
print(performance_data)

if performance_data:
    create_line_graph(performance_data, data_sizes_order, 'insert', 'Insert Performance vs Data Size', 'Insert Time (secs)', 'logs/line_insert_performance')
    create_line_graph(performance_data, data_sizes_order, 'query', 'Query Performance vs Data Size', 'Query Time (secs)', 'logs/line_query_performance')
    create_line_graph(performance_data, data_sizes_order, 'successor', 'Successor Query Performance vs Data Size', 'Successor Time (secs)', 'logs/line_successor_performance')
    # create_construction_bar_graph(performance_data, data_sizes_order, 'vEB-tree Construction Time vs Data Size', 'Build Time (secs)', 'logs/bar_construction_time')
    create_line_graph(performance_data, data_sizes_order, 'delete', 'Half Deletion Performance', 'Delete Time (secs)', 'logs/delete_performance')

else:
    print("No performance data parsed from log files.")