// --modified by Wangsi
#include "main.h"
#include "TrajectoryPrediction.h"
double calSphereDist(double lat1,double lon1, double lat2, double lon2) // lat_1�ĵ�λ�Ƕ�
{
	// �����Բ���߾��룺cos(delta_theta)=sin(lat1)*sin(lat2)+cos(lat1)*cos(lat2)*cos(delta_longitude), dist = R*delta_theta_(��λ��m)
	double dist = 0.0; // ��λ��m
	double delta_longitude = (lon1 - lon2)*PI/180; // ���Ȳ�
	double costheta;
	
	lat1 = lat1*PI/180;
	lat2 = lat2*PI/180;
	
	// ��������֮��ļнǣ��Ե�������Ϊԭ�㣩
	costheta = sin(lat1)*sin(lat2) + cos(lat1)*cos(lat2)*cos(delta_longitude);
		
	// ��ֹ���ھ������⣬����theta�ľ���ֵ����1
	if(costheta>1){
		costheta=1;
	}else	if(costheta<-1){
		costheta=-1;
	}
	
	dist = acos(costheta);
	dist = 6371000 * dist; // costheta��ֵ��ȷ��1e-6��ʱ��dist�������Կ�����10������
	return dist;
}

int sign(double x){
	// ��x�ķ���
	if(x>0){
		return 1;
	}else if(x==0){
		return 0;
	}else{
		return -1;
	}
}

long myRound(double x){
	// ��x��������
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
		
		if((-x-tmpx)>=0.5)
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
	// N����ֵ�˲�(N<=N_medfilt)��������x[N]�������򣬲����������м����ֵ
	if(N>N_medfilt){
		//__breakpoint(0); // ������ֵ�˲��ĵ������ܳ���Ԥ��ֵ
		return -1;
	}
	// ð������
	bubbleSort(x,N);
	return x[N/2];	
}

int TrajectoryPrediction(char * gpsMsg, GPS_INFO *GPS){
	// ����GPS��γ����Ϣ���к��������Ԥ�⣬�����µ�����д��flash
	//--modified by Wangsi	
	int idx = 0; // ��ʱѭ����
	int idx_t = 0;
	int idx_start = 0; // gps�����������ʼ��
	int idx_XY = 0;
	
	unsigned int len_XY = 0; // ��Ч���ݵĳ��ȣ�С�ڵ���len_gps_data��
	double lon0,lat0; // ÿ�ι��ƺ���ʱ���õ���ʼ�������
	u32 gps_sec; // UTCʱ�䣬��λ���루ת��Ϊ�й���ʱ��Ҫ��8Сʱ��
	double tmp, mat1_11, mat1_12, mat1_21, mat1_22, mat2_11, mat2_12, mat2_21, mat2_22, mat3_11, mat3_21;
	double A11;
	double theta,theta_,theta_net;
	double offset_y,offset_x;
	double sin_tmp,cos_tmp;
	double dist_tmp;
	double lon_tmp, lat_tmp;
	u8 is_data_valid = 0; // �ж��½������ݵ��Ƿ���Ч
	u8 N = 7;
	double min_dist = 42.0; // ���½����ݵ��֮ǰN�����С���룬���С�ڸ�ֵ�������������
	char *buf;
	
	u8 status;
	u8 idxcomma2;
	
	buf = strstr(gpsMsg,"$GPRMC"); // bufָ��ָ����Ϣ����ǰ�ˣ���������ڣ���ΪNULL
	idxcomma2 = GetComma(2,buf);	
			
	if(buf!=NULL && idxcomma2!=0){
		status = buf[idxcomma2]; // ����еڶ������ź���ַ�
	}else{
		status = 0;
	}
	
	if (buf!=NULL && status=='A')  //($GPRMC)
	{	
		flag_gps_data_available = 1; // �ɹ���������Ч��GPS����
		
		GPS->latitude  = Get_Double_Number(&buf[GetComma( 3, buf)]);
		GPS->longitude = Get_Double_Number(&buf[GetComma( 5, buf)]);
/*
gps_sec = ((buf[7]-'0')*10+(buf[8]-'0'))*3600 + ((buf[9]-'0')*10+(buf[10]-'0'))*60 + ((buf[11]-'0')*10+(buf[12]-'0'));	// UTCʱ�䣬��λ���루ת��Ϊ�й���ʱ��Ҫ��8Сʱ��
printf("sec: %d\n",gps_sec);
printf("lat: %d��%f��\n",(int)(GPS->latitude/600000.0),(GPS->latitude/600000.0-(int)(GPS->latitude/600000.0))*60.0);
printf("lon: %d��%f��\n",(int)(GPS->longitude/600000.0),(GPS->longitude/600000.0-(int)(GPS->longitude/600000.0))*60.0);
*/
		
		if(strlen(buf)>12){
			GPS->second = ((buf[7]-'0')*10+(buf[8]-'0'))*3600/2 + ((buf[9]-'0')*10+(buf[10]-'0'))*60/2 + ((buf[11]-'0')*10+(buf[12]-'0'))/2;	//��2��Ϊ��λ
		}else{
			GPS->second = -1;
		}

		GPS->NS = buf[GetComma(4, buf)];
		GPS->EW = buf[GetComma(6, buf)];
		
		
		//���¾�γ����Ϣ
		jingdu = GPS->longitude; 
		weidu  = GPS->latitude;	
				
		//���٣�time_o��backupŲ������
		if(GetComma( 7, buf)!=0){
			sog = Get_Double_Number_sog(&buf[GetComma(7, buf)]);
		}
		
		//����
		Read_Flash_Cogsel();
		
		// ����������GPS��γ����ʷ���ݹ��ƺ���--modified by Wangsi
		if( is_gps_data_init==TRUE && idx_tail==(len_gps_data-1) ){
			is_gps_data_init = FALSE; // �����ʼ��״̬
		}
		
		// �����½������ݣ��õ�lat_tmp, lon_tmp��gps_sec
		if(GPS -> NS == 'N'){
			lat_tmp =  GPS->latitude/600000.0; // ��γΪ��
		}else if(GPS -> NS == 'S'){
			lat_tmp = -GPS->latitude/600000.0; // ��γΪ��
		}				
		if(GPS -> EW == 'E'){
			lon_tmp =  GPS->longitude/600000.0; // ת��Ϊ�ȣ�jingdu�ĵ�λ��1/10000�֣�����Ϊ��
		}else if(GPS -> EW == 'W'){
			lon_tmp = -GPS->longitude/600000.0; // ����Ϊ��
		}	
		if(strlen(buf)>12){	
			gps_sec = ((buf[7]-'0')*10+(buf[8]-'0'))*3600 + ((buf[9]-'0')*10+(buf[10]-'0'))*60 + ((buf[11]-'0')*10+(buf[12]-'0'));	// UTCʱ�䣬��λ���루ת��Ϊ�й���ʱ��Ҫ��8Сʱ��
		}else{
			gps_sec = -1;
		}
		
		gps_data_available_cnt++; // ��¼�Ѿ��յ�����Чgps���ݸ���
		
		// �������ݵ�浽gps_lat_tmp��gps_lon_tmp�У���(���б�Ҫ)�������ϵ�����
		// idx_medfilt����ָ�������е����һ����Ч����
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
		
		// ���½����ݽ�����ֵ�˲�
		for(idx=0;idx<=idx_medfilt;idx++){
			gps_lat_tmp1[idx] = gps_lat_tmp[idx];
			gps_lon_tmp1[idx] = gps_lon_tmp[idx];
		}
		lat_tmp = gpsDataMedfilt(gps_lat_tmp1,idx_medfilt+1);
		lon_tmp = gpsDataMedfilt(gps_lon_tmp1,idx_medfilt+1); 
		
		// �ж��½��������Ƿ�����Ҫ���粻����Ҫ��������
		is_data_valid = 0; // Ĭ������
		if( is_gps_data_init && idx_tail<N-1 ){ //�ѱ���ĵ���������N-1,�½��������ݵ�����������
				is_data_valid = 1;
		}else if(calSphereDist(lat_tmp,lon_tmp,gps_latitude[(idx_tail-N+1)%len_gps_data],gps_longitude[(idx_tail-N+1)%len_gps_data])>min_dist){ // ����(x_n,y_n)��(x_n-N,y_n-N)֮��ľ��룬������min_dist�������õ�
			is_data_valid = 1;
		}
			
		// ���������Ч����ˢ�����ݣ������к������λ��λ�ù���
		if(is_data_valid){		
			//printf("data valid\n");
			idx_tail = (idx_tail+1)%len_gps_data;
			gps_latitude[idx_tail]  = lat_tmp;
			gps_longitude[idx_tail] = lon_tmp;
			gps_time_tag[idx_tail]  = gps_sec;
			
			if(is_gps_data_init==TRUE){
				// ��ʼ���׶�					
				idx_start = 0;							
			}else if(is_gps_data_init==FALSE){
				// ����׶�
				idx_start = (idx_tail+1)%len_gps_data;					
			}
			
			idx_t = idx_start; // ������汾�ĳ�������б��������ĵ㶼����Ч��
			
			lon0 = gps_longitude[idx_t]; // �Ե�һ����Ч������Ϊ����ԭ��
			lat0 = gps_latitude[idx_t];

			while(idx_t!=idx_tail){
				X[idx_XY] = sign(gps_longitude[idx_t]-lon0)*calSphereDist(lat0,lon0,lat0,gps_longitude[idx_t]); // ���Ƽ���X����					
				Y[idx_XY] = sign(gps_latitude[idx_t]-lat0)*calSphereDist(lat0,lon0,gps_latitude[idx_t],lon0);   // ���Ƽ���Y����
				idx_XY = (idx_XY+1)%len_gps_data;
				idx_t = (idx_t+1)%len_gps_data;
			}
			X[idx_XY] = sign(gps_longitude[idx_t]-lon0)*calSphereDist(lat0,lon0,lat0,gps_longitude[idx_t]);
			Y[idx_XY] = sign(gps_latitude[idx_t]-lat0)*calSphereDist(lat0,lon0,gps_latitude[idx_t],lon0);
			len_XY = idx_XY + 1;
			
			if(len_XY>=3){ // ������Ҫ3�����ݲ��ܹ���	
				//printf("len_XY>=3\n");						
			
				// ����mat1 = X.'*X (2*2 matrix)
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
					
				// ����mat2 = inv(mat1) (2*2 matrix)
				tmp = mat1_11*mat1_22-mat1_12*mat1_21;
				mat2_11 = mat1_22/tmp;
				mat2_12 = -mat1_12/tmp;
				mat2_21 = -mat1_21/tmp;
				mat2_22 = mat1_11/tmp;
				
				// ����mat3 = X.'*Y (2*1 matrix)
				mat3_11 = 0;
				mat3_21 = 0;
				
				for(idx=0;idx<len_XY;idx++){
					mat3_11 += X[idx]*Y[idx];
				}
				
				for(idx=0;idx<len_XY;idx++){
					mat3_21 += Y[idx];
				}
				
				// ����A = inv(X.'*X)*X.'*Y(��mat2*mat3, 2*1 matrix)
				A11 = mat2_11*mat3_11 + mat2_12*mat3_21;
						
				theta_ = atan(A11); // theta_����x������ļнǣ��˴��ķ�Χ-PI/2~PI/2						

				// �������һ����Ч���ݵ�����ȷ�����򣬽�theta_�ķ�Χ��չ��-PI~PI
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
				// ����������������ļнǣ�˳ʱ����ת�Ƕ����󣬷�Χ0~2PI
				if(theta_>=-PI && theta_<PI/2){
					theta = 0.5*PI-theta_;
				}else{
					theta = 2.5*PI-theta_;
				}
				
				if(offset_len1==0){
					offset_len1 = 0.001; // ��ֹƫ�ù�С��Ϊ0ʱ��������atan���bug
				}
				offset_theta = atan(offset_len2/offset_len1);

				if(tmp!=0 && !isnan(theta) && !isnan(theta_) && !isnan(offset_theta)){ // ֻ�е����Ƴ���theta��direction�ȷ�����ʱ���Ž��б��θ���
					// printf("calculating direction\n");
					direction = myRound( (theta/PI*180) * 10 ); // ����������ļнǣ���λ0.1��

					// ���ݺ��������ƫ��������������λ��							
					offset_len3 = sqrt(offset_len1*offset_len1+offset_len2*offset_len2);
					offset_y = offset_len3*sin(theta_+offset_theta);
					offset_x = offset_len3*cos(theta_+offset_theta);
					weidu = (gps_latitude[idx_tail]-offset_y/6371000/PI*180)*600000.0;
					sin_tmp = sin(gps_latitude[idx_tail]/180*PI);
					cos_tmp = cos(gps_latitude[idx_tail]/180*PI);
					// �����������ķ����
					theta_net = theta-offset_theta;
					if(theta_net>2*PI){
						theta_net = theta_net-2*PI;
					}else if(theta_net<-2*PI){
						theta_net = theta_net+2*PI;
					}
					if( theta_net>=0 && theta_net<PI ){
						jingdu = ( gps_longitude[idx_tail] - acos((cos(offset_x/6371000)-sin_tmp*sin_tmp)/cos_tmp/cos_tmp)/PI*180 )*600000.0;								
					}else{
						jingdu = ( gps_longitude[idx_tail] + acos((cos(offset_x/6371000)-sin_tmp*sin_tmp)/cos_tmp/cos_tmp)/PI*180 )*600000.0;
					}
					
					// ��¼���һ�ι��Ƴ��ľ�γ��
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
		}else{
			// ����µ����ݵ���Ч��������һ�γɹ����Ƴ������ݵ�
			jingdu = jingdu_tmp;
			weidu  = weidu_tmp;
			direction = direction_tmp;
		}

	}
	
	return 0;
}




