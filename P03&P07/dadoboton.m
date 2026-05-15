if ~exist("s","var")
s = serialport("COM3", 115200);
configureTerminator(s, "LF");
end

tiempo_total = 0;
cara = 0;
t_last_display = tic; 
t_loop = tic; 
fprintf("INICIANDO.\n");

while true
    data = readline(s);
    values = str2double(split(data, ","));
    
    if numel(values) == 4
        ax = values(1); ay = values(2); az = values(3);
        estado = values(4);
        dt = toc(t_loop); 
        t_loop = tic; 
        
        if estado == 1
            tiempo_total = tiempo_total + dt;
        else
            tiempo_total = 0; 
        end

       if abs(ax) > abs(ay) && abs(ax) > abs(az)
        % El eje X es el mayor
        if ax > 0 
            cara = 2; 
        else 
            cara = 5; 
        end
            elseif abs(ay) > abs(ax) && abs(ay) > abs(az)
        % El eje Y es el mayor
        if ay > 0 
            cara = 3; 
        else 
            cara = 4; 
        end
        else
        % El eje Z es el mayor
        if az > 0, cara = 1; else 
            cara = 6; 
        end
       end
    
        if toc(t_last_display) > 0.2
            clc;
            fprintf("/*/*/*/*/*//*/*/\n");
            fprintf("/*/  DADO  /*/\n");
            fprintf("/*/*/*/*/*/*/*/\n");
            
            
            if estado == 1
                msg_boton = "SET"; 
            else
                msg_boton = "RESET";
            end
            
            fprintf("BOTON: %s\n", msg_boton);
            fprintf("TIEMPO: %.1f s\n", tiempo_total);
            fprintf("*************\n");
            fprintf("* CARA: %d  *\n", cara);
            fprintf("*************\n");
            fprintf("Acelerometro: ax:%d | ay:%d | az:%d\n", ax, ay, az);
            
            t_last_display = tic; 
        end
    end
end
