clear
clc
close all

%Caracteristicas de la comunicacón
puerto = 'COM5'; %Puerto usado por Arduino
baudRate = 115200;
Muestra_rev = 45; %Puntos por cada vuelta, considerando 4.5 segundos para una vuelta
Vueltas_t = 2; 
Muestras_t = Muestra_rev * Vueltas_t; %Se toman datos de dos vueltas completas
Distancias = zeros(1, Muestras_t); %Crear valores iniciales para los vectores
Angulos = linspace(0, 2*pi * Vueltas_t, Muestras_t); %Crear vector para los angulos

%Abrir puerto
s = serialport(puerto, baudRate);
configureTerminator(s, "LF");
flush(s);

fprintf('Puerto %s conectado. \nComenzar a rotar el objeto...\n', puerto);

figure(1);
%Medición de distancia respecto al tiempo
subplot(1,2,1);
hLine = animatedline('Color', [0.8500 0.3250 0.0980], 'LineWidth', 1);
grid on; title('Distancia');
xlabel('Muestra [n]'); ylabel('Distancia [mm]');
ylim([0 500]); 

%Reconstrucción 2D con transformación polar
subplot(1,2,2);
hPolar = polarplot(0, 0, 'o', 'MarkerSize', 3, 'MarkerFaceColor', 'b');
title('Transformación Polar');
grid on;

%Captura de datos
for i = 1:Muestras_t
    try
        linea = readline(s);        
        %Extraer el valor de la distancia filtrada con la cadena de salida
        datos = sscanf(linea, 'Distancia cruda (mm): %f | Distancia filtrada (mm): %f');
        if ~isempty(datos)
            r = datos(2); %r toma los datos normales y filtrados, al almacenar en un vector considerando los filtrados (2)
            Distancias(i) = r;
            addpoints(hLine, i, r); %Se actualiza el grafico de linea
            %Se grafica hasta i
            set(hPolar, 'ThetaData', Angulos(1:i), 'RData', Distancias(1:i)); %Actualizar datos de transformación polar
            
            drawnow limitrate;
        end
    catch
        continue;
    end
end

fprintf('Reconstrucción terminada.');
clear s; %Cerramos el puerto serial
