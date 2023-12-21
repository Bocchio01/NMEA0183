classdef MWV < handle
    % MWV Sensor class

    properties
        data
        fig
        plotter
        annotation
    end

    methods
        function MWV = MWV(fig)
            
            MWV.data = struct( ...
                'angle', 0.0, ...
                'speed', 0.0, ...
                'unit', '', ...
                'status', '');

            if ~ishandle(fig)
                fig = figure('Name', 'MWV Telemetry', 'NumberTitle', 'off');
            end
            MWV.fig = fig;

            nexttile
            MWV.plotter = polarplot(0, 0, "-o", ...
                "LineWidth", 2);
            rlim([0 100]);
            grid on
            title('MWV')
            legend('Wind speed and angle')

            % MWV.annotation = annotation('textbox', ...
            %     [0 .7 .3 .3], ...
            %     'FitBoxToText', 'on');

        end

        function [MWV] = Parser(MWV, data_from_STM)

            anglePattern = 'Angle:\s+(\d+\.\d+)';
            speedPattern = 'Speed:\s+(\d+\.\d+)';
            unitPattern = 'Unit:\s+(\w+)';
            statusPattern = 'Status:\s+(\w+)';

            angleMatch = regexp(data_from_STM, anglePattern, 'tokens', 'once');
            speedMatch = regexp(data_from_STM, speedPattern, 'tokens', 'once');
            unitMatch = regexp(data_from_STM, unitPattern, 'tokens', 'once');
            statusMatch = regexp(data_from_STM, statusPattern, 'tokens', 'once');

            try
                MWV.data.angle = str2double(angleMatch{1});
                MWV.data.speed = str2double(speedMatch{1});
                MWV.data.unit = unitMatch{1};
                MWV.data.status = statusMatch{1};
            catch
                disp('error reading for MWV')
            end
        end

        function [] = Plotter(MWV)

            theta = deg2rad(MWV.data.angle);
            r = MWV.data.speed;

            MWV.plotter.ThetaData = [MWV.plotter.ThetaData theta];
            MWV.plotter.RData = [MWV.plotter.RData r];

            % annotation_text = sprintf('MWV Data:\nAngle: %.2f°\nSpeed: %.2f %s\nStatus: %s', ...
            %     MWV.data.angle, ...
            %     MWV.data.speed, ...
            %     MWV.data.unit, ...
            %     MWV.data.status);
            % 
            % set(MWV.annotation, 'String', annotation_text)

            drawnow;

        end
    end
end

