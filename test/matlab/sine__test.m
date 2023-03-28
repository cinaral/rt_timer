%* setup
bin_dir = '../../build/bin';
dat_dir = '../../build/dat';
ref_dat_dir = "../reference_dat";
test_name = 'sine-test';
dat_prefix = append(dat_dir, '/', test_name, '-');
ref_dat_prefix = append(ref_dat_dir, '/', test_name, '-');
exe_name = append(test_name, '.exe');
t_arr_fname = 't_arr.dat';
x_arr_fname = 'x_arr.dat';

is_drawing = false;
is_single_precision = false;
t_dim = 1000;
f = 1;

if is_single_precision
	error_thres = 1e-6; %* single precision
else 
	error_thres = 1e-14;
end

%* create reference data 
t_arr_chk = linspace(0, 1, t_dim).';
x_arr_chk = sin(t_arr_chk * 2 * pi * f);

writematrix(t_arr_chk, append(ref_dat_prefix, t_arr_fname));  
writematrix(x_arr_chk, append(ref_dat_prefix, x_arr_fname));  

disp(append('Created reference data for ', test_name));

prev_pwd = pwd;
cd(bin_dir);
if isfile(exe_name)
	%* call the test executable
	if system(exe_name) > 0
		warning(append(bin_dir, '/', exe_name, ' has returned failure.'));
	end
	
	%* read the results
	t_arr = readmatrix(append(dat_prefix, t_arr_fname));
	x_arr = readmatrix(append(dat_prefix, x_arr_fname));

	%* verify
	t_max_error = max(t_arr - t_arr_chk);
	t_mean_error = mean(t_arr - t_arr_chk);

	x_max_error = max(vecnorm(x_arr - x_arr_chk, 2, 2));
	x_mean_error = mean(vecnorm(x_arr - x_arr_chk, 2, 2));

	if x_max_error < error_thres
		disp(append(test_name, '	ok'));
	else
		disp(append(test_name, '	fail'));
	end

	if is_drawing
		figure('Name', 'test-sine x_arr');
		hold on;
		plot(t_arr, x_arr);
		plot(t_arr_chk, x_arr_chk, '--');
	end
else
	error(append(bin_dir, '/', exe_name, ' does not exist. Use CMake to build the test.'));
end
cd(prev_pwd);
