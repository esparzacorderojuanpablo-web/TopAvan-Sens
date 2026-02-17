load("/MATLAB Drive/MobileSensorData/1.mat")

lat = Position.latitude;
lon = Position.longitude;

lat_rad = deg2rad(lat);
lon_rad = deg2rad(lon);

R = 6371000;

dlat = diff(lat_rad);
dlon = diff(lon_rad);

a = sin(dlat/2).^2 + cos(lat_rad(1:end-1)) .* cos(lat_rad(2:end)) .* sin(dlon/2).^2;
c = 2 * atan2(sqrt(a), sqrt(1-a));
d = R * c;

dt = 1;
velocidad = d/dt;
vel_kmh = velocidad * 3.6;

mean(vel_kmh)
std(vel_kmh)
max(vel_kmh)


plot(vel_kmh)
xlabel("Muestras")
ylabel("Velocidad (km/h)")
title("Velocidad calculada - 10 Hz")
grid on