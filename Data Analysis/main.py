import pandas as pd
import matplotlib.pyplot as plt


file_name = '1_DATALOG.csv'

# Read the CSV file into a pandas DataFrame
df = pd.read_csv(f'Data Logged/{file_name}', delimiter=';')

# Extract the useful information
temperature = df['Temperature']
pressure = df['Pressure']
altitude = df['Altitude']
acceleration = df[['AccX', 'AccY', 'AccZ']]

# # Print the extracted information
# print("Temperature:")
# print(temperature)
# print()

# print("Pressure:")
# print(pressure)
# print()

# print("Altitude:")
# print(altitude)
# print()

# print("Acceleration:")
# print(acceleration)
# print()

# Plot the data
# Example plot: Temperature over time
plt.plot(temperature)
plt.xlabel('Time')
plt.ylabel('Temperature')
plt.title('Temperature Variation')
plt.show()

# Example plot: Pressure vs. Altitude
plt.plot(pressure, altitude)
plt.xlabel('Pressure')
plt.ylabel('Altitude')
plt.title('Pressure vs. Altitude')
plt.show()

# Example plot: Acceleration components
time = range(len(acceleration))
plt.plot(time, acceleration['AccX'], label='AccX')
plt.plot(time, acceleration['AccY'], label='AccY')
plt.plot(time, acceleration['AccZ'], label='AccZ')
plt.xlabel('Time')
plt.ylabel('Acceleration')
plt.title('Acceleration Components')
plt.legend()
plt.show()


# Example plot: Acceleration magnitude
acceleration_magnitude = (
    acceleration['AccX']**2 +
    acceleration['AccY']**2 +
    acceleration['AccZ']**2)**0.5
plt.plot(time, acceleration_magnitude)
plt.xlabel('Time')
plt.ylabel('Acceleration')
plt.title('Acceleration Magnitude')
plt.show()
