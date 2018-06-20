#include "map_test.h"
#include "var.h"

#include "posix_atomic.h"
#include "posix_ifos.h"
#include "posix_string.h"

//max 100 MB memorage
#define DATA_STREAM_SIZE (100*1024*1024)


int var__dump2file() {
	var__map_layout_t *g__layout;
	g__layout = var__get_layout();
	if( g__layout == NULL ) {
		printf("g__layout is null!\n");
		return -1;
	}
	
	const char *path = posix__getpedir();
	if( !path ) {
		printf("cur_path return null!\n");
		return -1;
	}
	
	//create file 
	int fd;
	char filepath[512];
	memset(filepath, 0, 512);
	char filename[16] = "layout.bin";
	posix__sprintf(filepath, cchof(filepath), "%s/%s", path, filename);
	printf("dump file full path:%s\n", filepath);
	if( posix__file_create(filepath, &fd) < 0 ){
		printf("create file %s failure!\n", filepath);
		return -1;
	}
	
	//Organization data 
	char *data_stream = (char*) malloc (DATA_STREAM_SIZE);
	if( !data_stream ) {
		posix__close(fd);
		printf("malloc size[%d] failure!\n", DATA_STREAM_SIZE);
		return -1;
	}
	size_t data_len = 0;
	//memset(data_stream, 0, DATA_STREAM_SIZE);
	//g__layout->nodes_
	memcpy(data_stream + data_len, &g__layout->nodes_.count_, sizeof(int));
	data_len += sizeof(int);
	memcpy(data_stream + data_len, (char*)(g__layout->nodes_.data_), g__layout->nodes_.count_ * sizeof ( var__map_node_t) );
	data_len += g__layout->nodes_.count_ * sizeof ( var__map_node_t);
	
	//g__layout->edges_
	memcpy(data_stream + data_len, &g__layout->edges_.count_, sizeof(int));
	data_len += sizeof(int);
	int i = 0;
	var__edge_t *p_edge;
	
	for( ; i < g__layout->edges_.count_; ++i) {
		p_edge = (var__edge_t *) &((var__edge_t*) g__layout->edges_.data_)[i];
		memcpy(data_stream + data_len, (char*)p_edge + sizeof(struct avltree_node_t), sizeof(int) * 3 + sizeof(position_t) * 4 + sizeof(double) );
		data_len += ( sizeof(int) * 3 + sizeof(position_t) * 4 + sizeof(double) );
		
		//var__edge_t->speed_limit_
		memcpy(data_stream + data_len, &p_edge->speed_limit_.count_, sizeof(int) );
		data_len += sizeof(int);
		if( p_edge->speed_limit_.count_ > 0 ) {
			memcpy(data_stream + data_len, (char*)(p_edge->speed_limit_.data_), p_edge->speed_limit_.count_ * sizeof(var__speed_limit_t) );
			data_len += p_edge->speed_limit_.count_ * sizeof(var__speed_limit_t);
		}
		
		//var__edge_t->disperse_points_
		memcpy(data_stream + data_len, &p_edge->disperse_points_.count_, sizeof(int) );
		data_len += sizeof(int);
		if( p_edge->disperse_points_.count_ > 0 ) {
			memcpy(data_stream + data_len, (char*)(p_edge->disperse_points_.data_), p_edge->disperse_points_.count_ * sizeof(position_t) );
			data_len += p_edge->disperse_points_.count_ * sizeof(position_t);
		}
		
		//var__edge_t->wops_
		memcpy(data_stream + data_len, &p_edge->wops_.count_, sizeof(int) );
		data_len += sizeof(int);
		if( p_edge->wops_.count_ > 0 ) {
			memcpy(data_stream + data_len, (char*)(p_edge->wops_.data_), p_edge->wops_.count_ * sizeof(var__edge_wop_properties_t) );
			data_len += p_edge->wops_.count_ * sizeof(var__edge_wop_properties_t);
		}
	}
	
	
	//g__layout->map_type_
	memcpy(data_stream + data_len, &g__layout->map_type_, sizeof(int));
	data_len += sizeof(int);
	
	//g__layout->wops_
	memcpy(data_stream + data_len, &g__layout->wops_.count_, sizeof(int));
	data_len += sizeof(int);
	var__way_of_pass_t *p_pass;
	for( i = 0; i < g__layout->wops_.count_; ++i ) {
		p_pass = (var__way_of_pass_t *) &((var__way_of_pass_t*) g__layout->wops_.data_)[i];
		memcpy(data_stream + data_len, (char*)p_pass + sizeof(struct avltree_node_t), sizeof(int) * 3 + sizeof(double) );
		data_len += (sizeof(int) * 3 + sizeof(double));
	}
	
	//g__layout->links_
	memcpy(data_stream + data_len, &g__layout->links_.count_, sizeof(int));
	data_len += sizeof(int);
	memcpy(data_stream + data_len, (char*)(g__layout->links_.data_), g__layout->links_.count_ * sizeof ( var__edge_link_t) );
	data_len += g__layout->links_.count_ * sizeof ( var__edge_link_t);
	
	//g__layout->wop_searcher_root_
	//g__layout->edge_searcher_root_
	
	
	//write data to file
	size_t write_size = posix__write_file(fd, data_stream, data_len);
	if( write_size != data_len ) {
		printf("write size %u, data stream size %u\n", write_size, data_len);
		posix__close(fd);
		return -1;
	}
	
	posix__close(fd);
	printf("dump memory to bin file success!!\n");
	return 0;
}

void var__free_layout() {
	var__end_search();
}
