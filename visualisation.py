
import matplotlib.pyplot as plt
import pandas as pd

def visualize_results(filename):
    data = pd.read_csv(filename)
    plt.figure(figsize=(10, 6))
    plt.bar(data['Test'], data['Time'])
    plt.title('Fixed Length Allocator Performance')
    plt.xlabel('Test Type')
    plt.ylabel('Time (seconds)')
    plt.show()

if __name__ == '__main__':
    visualize_results('results.csv')
