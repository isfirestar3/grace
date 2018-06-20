from ctypes import *
import pdb
import sys
import time
import struct

kControlorType_RealtimeViewer = (0x100 << 6)
kControlorType_Dispatcher = (0x100 << 7)

class var__status_describe_t(Structure):
	_pack_=1
	_fields_ = [
		("command_", c_uint32),
		("middle_", c_uint32),
		("response_", c_uint32)
	]

class upl_t(Structure):
	_pack_=1
	_fields_ = [
		("edge_id_", c_int),
		("percentage_", c_double),
		("angle_", c_double)
	]

class position_t(Structure):
	_pack_=1
	_fields_ = [
		("x_", c_double),
		("y_", c_double),
		("angle_", c_double)
	]

class var__vector_t(Structure):
	_pack_=1
	_fields_ = [
		("count_", c_int),
		("data_", c_uint64)
	]

class var__edge_wop_properties_t(Structure):
	_pack_=1
	_fields_ = [
		("wop_id_", c_int),
		("enabled_", c_char),
		("wop_properties_", c_int * 9)
	]

class navigation_t(Structure):
	_pack_=1
	_fields_ = [
		("max_speed_", c_double),
		("creep_speed_", c_double),
		("max_w_", c_double),
		("creep_w_", c_double),
		("slow_down_speed_", c_double),
		("acc_", c_double),
		("dec_", c_double),
		("dec_estop_", c_double),
		("acc_w_", c_double),
		("dec_w_", c_double),
		("creep_distance_", c_double),
		("creep_theta_", c_double),
		("upl_mapping_angle_tolerance_", c_double),
		("upl_mapping_dist_tolerance_", c_double),
		("upl_mapping_angle_weight_", c_double),
		("upl_mapping_dist_weight_", c_double),
		("tracking_error_tolerance_dist_", c_double),
		("tracking_error_tolerance_angle_", c_double),
		("aim_dist_", c_float),
		("predict_time_", c_uint32),
		("is_traj_whole_", c_double),
		("aim_angle_p_", c_double),
		("aim_angle_i_", c_double),
		("aim_angle_d_", c_double),
		("stop_tolerance_", c_double),
		("stop_tolerance_angle_", c_double),
		("stop_point_trim_", c_double),
		("aim_ey_p_", c_double),
		("aim_ey_i_", c_double),
		("aim_ey_d_", c_double),

		("track_status_", var__status_describe_t),

		("user_task_id_", c_uint64),
		("ato_task_id_", c_uint64),

		("dest_upl_", upl_t),
		("dest_pos_", position_t),
		("traj_ref_", var__vector_t),
		("pos_", position_t),
		("pos_time_stamp_", c_uint64),
		("pos_status_", c_uint32),
		("pos_confidence_", c_double),

		('traj_ref_index_curr_', c_int),
	     
		('upl_', upl_t),

		('tracking_error_', c_int),
		('base_point_', position_t),
		('aim_point_', position_t),
		('aim_heading_error_', c_double),
	    ('predict_point_', position_t),
	    ('predict_point_curvature_', c_double),
	    ('on_last_segment_', c_int),
		('dist_to_partition_', c_double),
		('dist_to_dest_', c_double),
	     	 
		('current_edge_wop_properties_', var__edge_wop_properties_t), 
		('current_task_id_', c_uint64),  
		('runtime_limiting_velocity_', c_double)
	]
	 
class test(Structure):
	_pack_=1
	_fields_ = [('a', c_int),('b', c_int)]

def mycallback(ptr, n, pv):
	#print(type(ptr))
	#print(ptr.contents)
	for i in range(n):
		print(ptr[i])
	# s = struct.unpack('s', ptr)
	# print(s)
	pv[0] = 101
	return 10001

def main():
	# add path to search nshost
	# sys.path.append('/usr/local/lib64')

	dynamicLibString='./pynet.so'
	mylib=cdll.LoadLibrary(dynamicLibString)

	callback_t = CFUNCTYPE(c_int, POINTER(c_byte), c_int, POINTER(c_int))
	pfunc = callback_t(mycallback);
	mylib.py_test_pointer_stream(pfunc);
	return

	t1=test()
	t1.a = t1.b = 10
	t2=test()
	t2.a = t2.b = 20
	t3=test()
	t3.a = t3.b = 30

	input_parameter_t = test * 3 # class PyCArrayType
	my_input = input_parameter_t()
	my_input[0] = t1
	my_input[1] = t2
	my_input[2] = t3
	mylib.py_test_pointer_array(byref(my_input), 3)

	print(type(mylib))
	# mylib.init_net.restype = c_int;
	robotid=mylib.py_init_net()
	print(robotid)

	# login to mt
	retval = mylib.py_login_to_host(robotid, b'192.168.1.64:4409', kControlorType_RealtimeViewer)
	if retval < 0:
		return
	print('successful login to mt.')

#	mylib.py_read_navigation_sync.argvtype=[c_int, POINTER(navigation_t)]
	nav=navigation_t()
	while 1:

		mylib.py_read_navigation_sync(robotid, byref(nav))
		# pdb.set_trace()
		print(nav.pos_time_stamp_)
		# print('%d %d %d %ld %ld' %\
		#  (nav.contents.track_status_.command_, nav.contents.track_status_.middle_, \
		#  	nav.contents.track_status_.response_, nav.contents.user_task_id_, nav.contents.ato_task_id_))
#		mylib.py_test_navigation_data(byref(nav))
#		mylib.py_test_navigation_data(nav)
		time.sleep(1)

if __name__ == '__main__':
	main()
