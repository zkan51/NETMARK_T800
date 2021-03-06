// --modified by Wangsi
#include "main.h"
#include "TrajectoryPrediction.h"
double calSphereDist(double lat1,double lon1, double lat2, double lon2) // lat_1的单位是度
{
	// 计算大圆航线距离：cos(delta_theta)=sin(lat1)*sin(lat2)+cos(lat1)*cos(lat2)*cos(delta_longitude), dist = R*delta_theta_(单位是m)
	double dist = 0.0; // 单位是m
	double delta_longitude = (lon1 - lon2)*PI/180; // 经度差
	double costheta;
	
	lat1 = lat1*PI/180;
	lat2 = lat2*PI/180;
	
	// 计算两点之间的夹角（以地球球心为原点）
	costheta = sin(lat1)*sin(lat2) + cos(lat1)*cos(lat2)*cos(delta_longitude);
		
	// 防止由于精度问题，导致theta的绝对值超过1
	if(costheta>1){
		costheta=1;
	}else	if(costheta<-1){
		costheta=-1;
	}
	
	dist = acos(costheta);
	dist = 6371000 * dist; // costheta的值精确到1e-6的时候，dist的误差可以控制在10米以内
	return dist;
}

int sign(double x){
	// 求x的符号
	if(x>0){
		return 1;
	}else if(x==0){
		return 0;
	}else{
		return -1;
	}
}

long myRound(double x){
	// 对x四舍五入
	long tmpx;
		
	if(x>=0){
		tmpx = (long)x;
		if((x-tmpx)>=0.5)
			return tmpx+1;
		else
			return tmpx;
	}else{
		x = -x;
		tmpx = (long)x;
		
		if((x-tmpx)>=0.5)
			return -(tmpx+1);
		else
			return -tmpx;
	}
}

void bubbleSort(double* a, int n)
{
	int i, j;
	double temp;
	for (j = 0; j < n - 1; j++){
		for (i = 0; i < n - 1 - j; i++){
			if(a[i] > a[i + 1]){
				temp = a[i];
				a[i] = a[i + 1];
				a[i + 1] = temp;
			}
		}
	}
}

double gpsDataMedfilt(double* x, int N){
	// N点中值滤波(N<=N_medfilt)，对数组x[N]进行排序，并返回数组中间的数值
	if(N>N_medfilt){
		//__breakpoint(0); // 用于中值滤波的点数不能超过预设值
		return -1;
	}
	// 冒泡排序
	bubbleSort(x,N);
	return x[N/2];	
}

double aver(double *a, int len){//数组求平均
    double r = 0;
    int i;
    for(i = 0; i < len; i ++)
        r += a[i];
			r/=len;
    return r;
}

// 航速估计
void SOGPrediction(double lat_tmp,double lon_tmp,u32 gps_sec,int lenA){
	double dist;
	unsigned int idx_start;
	idx_tail_sog = (idx_tail_sog+1)%len_gps_data;
	if(len_tail_sog < len_gps_data){
		len_tail_sog = len_tail_sog +1;
	}

	gps_latitude_sog[idx_tail_sog]  = lat_tmp;
	gps_longitude_sog[idx_tail_sog] = lon_tmp;		
	gps_time_tag_sog[idx_tail_sog]  = gps_sec;
	
		if(len_tail_sog - 1 < lenA){
			lenA = len_tail_sog - 1;
		}
		idx_start = (len_gps_data+idx_tail_sog-lenA)%len_gps_data;
		dist = calSphereDist(gps_latitude_sog[idx_tail_sog],gps_longitude_sog[idx_tail_sog], gps_latitude_sog[idx_start], gps_longitude_sog[idx_start]);
		sog_double = dist/( (86400 + gps_time_tag_sog[idx_tail_sog] - gps_time_tag_sog[idx_start])%86400 ) * 19.43845;//3.6/1.852*10
		
		sog_tmp[idx_sog_tag] = sog_double;
		idx_sog_tag = (idx_sog_tag + 1) % sog_num;
		
		if(len_tail_sog > 20){
		//采用均值滤波		
		if(len_tail_sog-1 < sog_num){
			sog_double = aver(sog_tmp,len_tail_sog-1);
		}else{
			sog_double = aver(sog_tmp,sog_num);
		}
		
		// 强制转换成整数(AIS消息18中航速用10bit表示，单位0.1节，范围0~102.2节，102.3节不可用)	
		sog = (unsigned int)sog_double; 
		if(sog>1022){
			sog = 1022; // 超过102.2节的按102.2节算
		}
		
// 		printf("sec: %d  sog_gps: %d  sog:%f\n",gps_sec, sog_gps, sog_double);
// 		printf("t: %d lat: %f lon: %f\n",gps_time_tag_sog[idx_tail_sog],gps_latitude_sog[idx_tail_sog],gps_longitude_sog[idx_tail_sog]);
// 		printf("idx_XY-lenA : T: %d  lat: %f lon: %f\n",gps_time_tag_sog[idx_start],gps_latitude_sog[idx_start],gps_longitude_sog[idx_start]);
// 		printf("dist: %f  delta_t: %d\n",dist, (86400+gps_time_tag_sog[idx_tail_sog]-gps_time_tag_sog[idx_start])%86400);
	}
}

void CalNetLocation(double theta, double theta_){
	// 根据方向角、坐标和偏移量计算航向和拖网的经纬度
	double theta_net;
	double offset_y,offset_x;
	double sin_tmp,cos_tmp;
	
	// 计算并修改航向(全局变量direction)
	direction = myRound( (theta/PI*180) * 10 ); // 与正北方向的夹角，单位0.1°
		
	offset_len3 = sqrt(offset_len1*offset_len1+offset_len2*offset_len2); 	// 计算并修改斜边长度(全局变量offset_len3)
	offset_y = offset_len3*sin(theta_+offset_theta);
	offset_x = offset_len3*cos(theta_+offset_theta);
	weidu = (gps_latitude[idx_tail]-offset_y/6371000/PI*180)*600000.0;		// 计算并修改拖网的纬度(全局变量weidu)
	
	
	sin_tmp = sin(gps_latitude[idx_tail]/180*PI);
	cos_tmp = cos(gps_latitude[idx_tail]/180*PI);
	// 计算网到船的方向角theta_net
	theta_net = theta-offset_theta;
	if(theta_net>2*PI){
		theta_net = theta_net-2*PI;
	}else if(theta_net<-2*PI){
		theta_net = theta_net+2*PI;
	}
	// 计算并修改拖网的经度(全局变量jingdu)
	if( theta_net>=0 && theta_net<PI ){
		jingdu = ( gps_longitude[idx_tail] - acos((cos(offset_x/6371000)-sin_tmp*sin_tmp)/cos_tmp/cos_tmp)/PI*180 )*600000.0;								
	}else{
		jingdu = ( gps_longitude[idx_tail] + acos((cos(offset_x/6371000)-sin_tmp*sin_tmp)/cos_tmp/cos_tmp)/PI*180 )*600000.0;
	}
}

int TrajectoryPrediction(char * gpsMsg, GPS_INFO *GPS){
	// 根据GPS经纬度信息进行航迹航向的预测，并将新的数据写入flash
	//--modified by Wangsi	
	int idx = 0; 											// 临时循环用
	int idx_t = 0;
	int idx_start = 0; 								// gps数据数组的起始点
	int idx_XY = 0;
	int offset_len_changed = 0;
	
	unsigned int num_cog_sample = 0;
	unsigned int len_XY = 0; 					// 有效数据的长度（小于等于len_gps_data）
	double lon0,lat0; 								// 每次估计航迹时所用的起始点的坐标
	u32 gps_sec; 											// UTC时间，单位是秒（转换为中国的时间要加8小时）
	double tmp, mat1_11, mat1_12, mat1_21, mat1_22, mat2_11, mat2_12, mat2_21, mat2_22, mat3_11, mat3_21;
	double A11;
	double theta, theta_;
	
	int lenA;
	
	double dist_tmp;
	double lon_tmp, lat_tmp;
	u8 is_data_valid = 0; // 判断新进的数据点是否有效
	u8 N = 7;
	double min_dist = 42.0; // 用于COG计算的最新进数据点和之前N点的最小距离，如果小于该值，则表明区域内采样太密集，舍弃这个点
	char *buf;
	
	u8 status;
	u8 idxcomma2;
	
	flag_gps_data_available = 0;
	
	buf = strstr(gpsMsg,"$GPRMC"); // buf指针指向消息的最前端，如果不存在，则为NULL
	idxcomma2 = GetComma(2,buf);
			
	if(buf!=NULL && idxcomma2!=0){
		status = buf[idxcomma2]; // 语句中第二个逗号后的字符
	}else{
		status = 0;
	}
	
	if (buf!=NULL && status=='A')  //($GPRMC)
	{	
		flag_gps_data_available = 1; // 成功搜索到有效的GPS数据
		
		// 判断offset_len的值是否有变化
		if(offset_len1_tmp!=offset_len1 || offset_len2_tmp!=offset_len2){
			offset_len_changed = 1;
		}
		
		// 保存最近一次进入本函数时offset_len1和offset_len2的值
		offset_len1_tmp = offset_len1;
		offset_len2_tmp = offset_len2;
		
		GPS->latitude  = Get_Double_Number(&buf[GetComma( 3, buf)]);
		GPS->longitude = Get_Double_Number(&buf[GetComma( 5, buf)]);
		
		//获取时间
		GPS->UTCTime = Get_Double_Number_time(&buf[GetComma( 1, buf)]);
		GPS->UTCDate = Get_Double_Number_time(&buf[GetComma( 9, buf)]);
		
		GPS->NS = buf[GetComma(4, buf)];
		GPS->EW = buf[GetComma(6, buf)];
				
		//更新经纬度信息
		jingdu = GPS->longitude; 
		weidu  = GPS->latitude;
		
		//航速，time_o的backup挪作他用
		if(GetComma( 7, buf)!=0){
			sog_gps = Get_Double_Number_sog(&buf[GetComma(7, buf)]);
		}
		
		// 拖网，根据GPS经纬度历史数据估计航向--modified by Wangsi
		if( is_gps_data_init==TRUE && idx_tail==(len_gps_data-1) ){
			is_gps_data_init = FALSE; // 脱离初始化状态
		}
		
		// 整理新进的数据，得到lat_tmp, lon_tmp和gps_sec
		if(GPS -> NS == 'N'){
			lat_tmp =  GPS->latitude/600000.0; // 北纬为正
		}else if(GPS -> NS == 'S'){
			lat_tmp = -GPS->latitude/600000.0; // 南纬为负
		}				
		if(GPS -> EW == 'E'){
			lon_tmp =  GPS->longitude/600000.0; // 转换为度，jin gdu的单位是1/10000分，东经为正
		}else if(GPS -> EW == 'W'){
			lon_tmp = -GPS->longitude/600000.0; // 西经为负
		}	
		if(strlen(buf)>12){	
			gps_sec = ((buf[7]-'0')*10+(buf[8]-'0'))*3600 + ((buf[9]-'0')*10+(buf[10]-'0'))*60 + ((buf[11]-'0')*10+(buf[12]-'0'));	// UTC时间，单位是秒（转换为中国的时间要加8小时）
		}else{
			gps_sec = -1;
		}
		
		gps_data_available_cnt++; // 记录已经收到的有效gps数据个数
		
		// 将新数据点存到gps_lat_tmp和gps_lon_tmp中，并(如有必要)丢弃最老的数据
		// idx_medfilt最终指向数组中的最后一个有效数据
		if(idx_medfilt<=N_medfilt-2){
			idx_medfilt++;
			gps_lat_tmp[idx_medfilt] = lat_tmp;
			gps_lon_tmp[idx_medfilt] = lon_tmp;
		}else if(idx_medfilt==N_medfilt-1){
			for(idx_medfilt=0;idx_medfilt<=N_medfilt-2;idx_medfilt++){
				gps_lat_tmp[idx_medfilt] = gps_lat_tmp[idx_medfilt+1];
				gps_lon_tmp[idx_medfilt] = gps_lon_tmp[idx_medfilt+1];
			}
			gps_lat_tmp[idx_medfilt] = lat_tmp;
			gps_lon_tmp[idx_medfilt] = lon_tmp;
		}
		
		// 对新进数据进行中值滤波
		for(idx=0;idx<=idx_medfilt;idx++){
			gps_lat_tmp1[idx] = gps_lat_tmp[idx];
			gps_lon_tmp1[idx] = gps_lon_tmp[idx];
		}
		lat_tmp = gpsDataMedfilt(gps_lat_tmp1,idx_medfilt+1);
		lon_tmp = gpsDataMedfilt(gps_lon_tmp1,idx_medfilt+1); 
		
		//不管数据是否有效，都进入速率计算
		
			lenA = sog_sample_interval;
			SOGPrediction(lat_tmp,lon_tmp,gps_sec,lenA);
		
		// 判断新进的数据是否满足要求，如不符合要求，则舍弃
		is_data_valid = 0; // 默认舍弃
		if( is_gps_data_init && idx_tail<N-1 ){ //已保存的点数不超过N-1,新进来的数据点无条件保留
				is_data_valid = 1;
		}else if(calSphereDist(lat_tmp,lon_tmp,gps_latitude[(idx_tail-N+1)%len_gps_data],gps_longitude[(idx_tail-N+1)%len_gps_data])>min_dist){ // 计算(x_n,y_n)和(x_n-N,y_n-N)之间的距离，若大于min_dist，则保留该点
			is_data_valid = 1;
		}
			
		// 如果数据有效，则刷新数据，并进行航向和网位仪位置估计
		if(is_data_valid){		
			//printf("data valid\n");
			idx_tail = (idx_tail+1)%len_gps_data;
			gps_latitude[idx_tail]  = lat_tmp;
			gps_longitude[idx_tail] = lon_tmp;
			gps_time_tag[idx_tail]  = gps_sec;
			
			if(is_gps_data_init==TRUE){
				// 初始化阶段					
				idx_start = 0;							
			}else if(is_gps_data_init==FALSE){
				// 常规阶段
				idx_start = (idx_tail+1)%len_gps_data;					
			}
			
			// 选取用于COG计算的数据的起始点（有效数据不足cog_sample_len个时，所有数据都用上；否则，用于计算的数据个数为cog_sample_len）
			if(cog_sample_len > len_gps_data){
				cog_sample_len = len_gps_data;	// 确保用于计算的数据点个数不超过所有可用的数据点个数
			}
			num_cog_sample = 0;
			for( idx_t = idx_tail; idx_t != idx_start; idx_t = ( (idx_t-1) + len_gps_data ) % len_gps_data ){
				num_cog_sample++;
				if(num_cog_sample==cog_sample_len){
					break;
				}
			}
			
			// 以第一个有效数据作为坐标原点
			lon0 = gps_longitude[idx_t]; 
			lat0 = gps_latitude [idx_t];

			// 从第一个参与计算的有效数据开始，计算后续的数据点的坐标
			while(idx_t!=idx_tail){
				X[idx_XY] = sign(gps_longitude[idx_t]-lon0)*calSphereDist(lat0,lon0,lat0,gps_longitude[idx_t]); // 近似计算X坐标					
				Y[idx_XY] = sign(gps_latitude[idx_t]-lat0)*calSphereDist(lat0,lon0,gps_latitude[idx_t],lon0);   // 近似计算Y坐标
				idx_XY 		= (idx_XY+1)%len_gps_data;
				idx_t 		= (idx_t+1)%len_gps_data;
			}
			X[idx_XY] = sign(gps_longitude[idx_t]-lon0)*calSphereDist(lat0,lon0,lat0,gps_longitude[idx_t]);
			Y[idx_XY] = sign(gps_latitude[idx_t]-lat0)*calSphereDist(lat0,lon0,gps_latitude[idx_t],lon0);
			len_XY 		= idx_XY + 1;
			
			if(len_XY>=3){ // 至少需要3个数据才能估计
				// 计算mat1 = X.'*X (2*2 matrix)
				mat1_11 = 0;
				mat1_12 = 0;
				mat1_21 = 0;
				mat1_22 = 0;											
				
				for(idx=0;idx<len_XY;idx++){
					mat1_11 += X[idx]*X[idx];
				}
				
				for(idx=0;idx<len_XY;idx++){
					mat1_12 += X[idx];
				}
				
				mat1_21 = mat1_12;
				mat1_22 = len_XY;
					
				// 计算mat2 = inv(mat1) (2*2 matrix)
				tmp = mat1_11*mat1_22-mat1_12*mat1_21;
				mat2_11 = mat1_22/tmp;
				mat2_12 = -mat1_12/tmp;
				mat2_21 = -mat1_21/tmp;
				mat2_22 = mat1_11/tmp;
				
				// 计算mat3 = X.'*Y (2*1 matrix)
				mat3_11 = 0;
				mat3_21 = 0;
				
				for(idx=0;idx<len_XY;idx++){
					mat3_11 += X[idx]*Y[idx];
				}
				
				for(idx=0;idx<len_XY;idx++){
					mat3_21 += Y[idx];
				}
				
				// 计算A = inv(X.'*X)*X.'*Y(即mat2*mat3, 2*1 matrix)
				A11 = mat2_11*mat3_11 + mat2_12*mat3_21;
						
				theta_ = atan(A11); // theta_是与x轴正向的夹角，此处的范围-PI/2~PI/2						

				// 根据最后一个有效数据的坐标确定方向，将theta_的范围扩展到-PI~PI
				if(theta_>=0 && theta_<=PI/2){			
					if( (X[idx_XY]+Y[idx_XY])>0 ){ 	// NorthEast
						theta_ = theta_;
					}else{
						theta_ = theta_-PI; 		// SouthWest
					}
				}else if(theta_>=-PI/2 && theta_<0){
					if( (X[idx_XY]-Y[idx_XY])>0 ){
						theta_ = theta_;			// SouthEast
					}else{
						theta_ = theta_+PI; 		// NorthWest
					}
				}
				// 航向是与正北方向的夹角，顺时针旋转角度增大，范围0~2PI
				if(theta_>=-PI && theta_<PI/2){
					theta = 0.5*PI-theta_;
				}else{
					theta = 2.5*PI-theta_;
				}
				
				if(offset_len1==0){
					offset_len1 = 0.001; // 防止偏置过小或为0时，下面求atan会出bug
				}
				offset_theta = atan(offset_len2/offset_len1);

				if(tmp!=0 && !isnan(theta) && !isnan(theta_) && !isnan(offset_theta)){ // 只有当估计出的theta和direction等非奇异时，才进行本次更新
					// 将新数据估计出的值赋值到全局变量保存起来
					TP_theta  = theta;
					TP_theta_ = theta_;
					
					// 根据航向、坐标和偏移量计算拖网的经纬度
					CalNetLocation(TP_theta, TP_theta_);
					
					// 记录最近一次估计出的经纬度
					jingdu_tmp = jingdu;
					weidu_tmp  = weidu;
					direction_tmp = direction;
				
					//dist_tmp = calSphereDist(gps_latitude[idx_tail],gps_longitude[idx_tail],weidu/600000.0,jingdu/600000.0);
					/*printf("%d %f %f %f %f\n", MMSI, dist_tmp, weidu/600000.0, jingdu/600000.0, direction/10.0 );		
					for(idx=idx_start;idx!=idx_tail;idx=(idx+1)%len_gps_data){
						printf("%f %f\n", gps_longitude[idx], gps_latitude[idx]);
					}*/
				}
			}//*/
		}else if(!offset_len_changed){
			// 如果新的数据点无效，且offset_len并没有被修改，则发送上一次成功估计出的数据点
			jingdu = jingdu_tmp;
			weidu  = weidu_tmp;
			direction = direction_tmp;
		}else{
			// 如果新的数据点无效，但offset_len被修改了，则需要重新计算拖网的经纬度
			CalNetLocation(TP_theta, TP_theta_);
			
			// 记录最近一次估计出的经纬度
			jingdu_tmp = jingdu;
			weidu_tmp  = weidu;
			direction_tmp = direction;
		}

	}
	
	// 将计算出的double型经纬度转换为AIS消息18需要的整型，并存储到flash中。如果为南纬或者西经，则将经纬度（负数）改为补码（28位）
	if(jingdu<0)
	{
		jingdu_flash = (unsigned long)(268435456 + jingdu);  //pow(2,28) = 268435456
	}
	else
	{
		jingdu_flash = (unsigned long)jingdu;
	}
	
	if(weidu <0)
	{
		weidu_flash = (unsigned long)(268435456 + weidu);
	}
	else
	{
		weidu_flash = (unsigned long)weidu;
	}
	
	return 0;
}




