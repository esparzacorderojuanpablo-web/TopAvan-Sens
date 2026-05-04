% ---------- CONFIGURACION DEL PUERTO ----------%
if ~exist("s","var")
s = serialport("COM3", 115200);
configureTerminator(s, "LF");
end
% ----------------------------------------------%
% ---------- CONFIGURACION DE UMBRALES ---------%
UMBRAL_ENTRADA = 16050; 
UMBRAL_SALIDA = 15100;  
% ----------------------------------------------%
cara = 0; 
t_loop = tic; 
t_last_display = tic;

while true
    data = readline(s);
    values = str2double(split(data, ","));
    
    if numel(values) == 4
        ax = values(1); ay = values(2); az = values(3);
        estado = values(4);
        

        dt = toc(t_loop); 
        
        if estado == 1
            tiempo_total = tiempo_total + dt; 
        else 
            tiempo_total = 0; 
        end

        accel_vec = [ax, ay, az];
        [val_max, indice] = max(abs(accel_vec));
        
        % Umbral
        if cara == 0
            umbral_actual = UMBRAL_ENTRADA; 
        else
            umbral_actual = UMBRAL_SALIDA;  
        end
% ------------ DETERMINAR LAS CARAS DEL DADO ------------%
        if val_max > umbral_actual
            % Actualizar si hay un valor mayor al umbral
            if indice == 3
                if az > 0, cara = 1; 
                else
                    cara = 6; 
                end
            elseif indice == 1
                if ax > 0
                    cara = 2; 
                else
                    cara = 5; 
                end
            elseif indice == 2
                if ay > 0
                    cara = 3; 
                else
                    cara = 4; 
                end
            end
        else
            cara = 0; % ESQUINA
        end
% ----------------------------------------------%
% --------IMPRIMIR EL ESTADO Y CARA DEL DADO -------%
        if toc(t_last_display) > 0.1
            clc;
            fprintf("/*/*/*/*/*//*/*/\n");
            fprintf("/*/  DADO  /*/\n");
            fprintf("/*/*/*/*/*/*/*/\n");
% -- IMPRIMIR SI ESTAMOS EN UNA ZONA INESTABLE O ESTABLE (rotando o en una esquina) --%
            if cara == 0
                fprintf("ESTADO: ESQUINA / ROTANDO\n");
            else
                fprintf("ESTADO: ESTABLE\n");
            end
% ----------------------------------------------%            
            fprintf("TIEMPO: %d SEGUNDOS\n", tiempo_total);
            fprintf("*************\n");
            fprintf("* CARA: %d  *\n", cara); 
            fprintf("*************\n");
% ----------------------------------------------%
            fprintf("Magnitud: %.0f | Umbral Actual: %d\n", val_max, umbral_actual);
            
            t_last_display = tic; 
        end
    end
end
