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
	// N����ֵ�˲�(N<=N_medfilt)��������x[N]�������򣬲����������м����ֵ
	if(N>N_medfilt){
		//__breakpoint(0); // ������ֵ�˲��ĵ������ܳ���Ԥ��ֵ
		return -1;
	}
	// ð������
	bubbleSort(x,N);
	return x[N/2];	
}

double aver(double *a, int len){//������ƽ��
    double r = 0;
    int i;
    for(i = 0; i < len; i ++)
        r += a[i];
			r/=len;
    return r;
}

// ���ٹ���
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
		//���þ�ֵ�˲�		
		if(len_tail_sog-1 < sog_num){
			sog_double = aver(sog_tmp,len_tail_sog-1);
		}else{
			sog_double = aver(sog_tmp,sog_num);
		}
		
		// ǿ��ת��������(AIS��Ϣ18�к�����10bit��ʾ����λ0.1�ڣ���Χ0~102.2�ڣ�102.3�ڲ�����)	
		sog = (unsigned int)sog_double; 
		if(sog>1022){
			sog = 1022; // ����102.2�ڵİ�102.2����
		}
		
// 		printf("sec: %d  sog_gps: %d  sog:%f\n",gps_sec, sog_gps, sog_double);
// 		printf("t: %d lat: %f lon: %f\n",gps_time_tag_sog[idx_tail_sog],gps_latitude_sog[idx_tail_sog],gps_longitude_sog[idx_tail_sog]);
// 		printf("idx_XY-lenA : T: %d  lat: %f lon: %f\n",gps_time_tag_sog[idx_start],gps_latitude_sog[idx_start],gps_longitude_sog[idx_start]);
// 		printf("dist: %f  delta_t: %d\n",dist, (86400+gps_time_tag_sog[idx_tail_sog]-gps_time_tag_sog[idx_start])%86400);
	}
}

void CalNetLocation(double theta, double theta_){
	// ���ݷ���ǡ������ƫ�������㺽��������ľ�γ��
	double theta_net;
	double offset_y,offset_x;
	double sin_tmp,cos_tmp;
	
	// ���㲢�޸ĺ���(ȫ�ֱ���direction)
	direction = myRound( (theta/PI*180) * 10 ); // ����������ļнǣ���λ0.1��
		
	offset_len3 = sqrt(offset_len1*offset_len1+offset_len2*offset_len2); 	// ���㲢�޸�б�߳���(ȫ�ֱ���offset_len3)
	offset_y = offset_len3*sin(theta_+offset_theta);
	offset_x = offset_len3*cos(theta_+offset_theta);
	weidu = (gps_latitude[idx_tail]-offset_y/6371000/PI*180)*600000.0;		// ���㲢�޸�������γ��(ȫ�ֱ���weidu)
	
	
	sin_tmp = sin(gps_latitude[idx_tail]/180*PI);
	cos_tmp = cos(gps_latitude[idx_tail]/180*PI);
	// �����������ķ����theta_net
	theta_net = theta-offset_theta;
	if(theta_net>2*PI){
		theta_net = theta_net-2*PI;
	}else if(theta_net<-2*PI){
		theta_net = theta_net+2*PI;
	}
	// ���㲢�޸������ľ���(ȫ�ֱ���jingdu)
	if( theta_net>=0 && theta_net<PI ){
		jingdu = ( gps_longitude[idx_tail] - acos((cos(offset_x/6371000)-sin_tmp*sin_tmp)/cos_tmp/cos_tmp)/PI*180 )*600000.0;								
	}else{
		jingdu = ( gps_longitude[idx_tail] + acos((cos(offset_x/6371000)-sin_tmp*sin_tmp)/cos_tmp/cos_tmp)/PI*180 )*600000.0;
	}
}

int TrajectoryPrediction(char * gpsMsg, GPS_INFO *GPS){
	// ����GPS��γ����Ϣ���к��������Ԥ�⣬�����µ�����д��flash
	//--modified by Wangsi	
	int idx = 0; 											// ��ʱѭ����
	int idx_t = 0;
	int idx_start = 0; 								// gps�����������ʼ��
	int idx_XY = 0;
	int offset_len_changed = 0;
	
	unsigned int num_cog_sample = 0;
	unsigned int len_XY = 0; 					// ��Ч���ݵĳ��ȣ�С�ڵ���len_gps_data��
	double lon0,lat0; 								// ÿ�ι��ƺ���ʱ���õ���ʼ�������
	u32 gps_sec; 											// UTCʱ�䣬��λ���루ת��Ϊ�й���ʱ��Ҫ��8Сʱ��
	double tmp, mat1_11, mat1_12, mat1_21, mat1_22, mat2_11, mat2_12, mat2_21, mat2_22, mat3_11, mat3_21;
	double A11;
	double theta, theta_;
	
	int lenA;
	
	double dist_tmp;
	double lon_tmp, lat_tmp;
	u8 is_data_valid = 0; // �ж��½������ݵ��Ƿ���Ч
	u8 N = 7;
	double min_dist = 42.0; // ����COG��������½����ݵ��֮ǰN�����С���룬���С�ڸ�ֵ������������ڲ���̫�ܼ������������
	char *buf;
	
	u8 status;
	u8 idxcomma2;
	
	flag_gps_data_available = 0;
	
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
		
		// �ж�offset_len��ֵ�Ƿ��б仯
		if(offset_len1_tmp!=offset_len1 || offset_len2_tmp!=offset_len2){
			offset_len_changed = 1;
		}
		
		// �������һ�ν��뱾����ʱoffset_len1��offset_len2��ֵ
		offset_len1_tmp = offset_len1;
		offset_len2_tmp = offset_len2;
		
		GPS->latitude  = Get_Double_Number(&buf[GetComma( 3, buf)]);
		GPS->longitude = Get_Double_Number(&buf[GetComma( 5, buf)]);
		
		//��ȡʱ��
		GPS->UTCTime = Get_Double_Number_time(&buf[GetComma( 1, buf)]);
		GPS->UTCDate = Get_Double_Number_time(&buf[GetComma( 9, buf)]);
		
		GPS->NS = buf[GetComma(4, buf)];
		GPS->EW = buf[GetComma(6, buf)];
				
		//���¾�γ����Ϣ
		jingdu = GPS->longitude; 
		weidu  = GPS->latitude;
		
		//���٣�time_o��backupŲ������
		if(GetComma( 7, buf)!=0){
			sog_gps = Get_Double_Number_sog(&buf[GetComma(7, buf)]);
		}
		
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
			lon_tmp =  GPS->longitude/600000.0; // ת��Ϊ�ȣ�jin gdu�ĵ�λ��1/10000�֣�����Ϊ��
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
		
		//���������Ƿ���Ч�����������ʼ���
		
			lenA = sog_sample_interval;
			SOGPrediction(lat_tmp,lon_tmp,gps_sec,lenA);
		
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
			
			// ѡȡ����COG��������ݵ���ʼ�㣨��Ч���ݲ���cog_sample_len��ʱ���������ݶ����ϣ��������ڼ�������ݸ���Ϊcog_sample_len��
			if(cog_sample_len > len_gps_data){
				cog_sample_len = len_gps_data;	// ȷ�����ڼ�������ݵ�������������п��õ����ݵ����
			}
			num_cog_sample = 0;
			for( idx_t = idx_tail; idx_t != idx_start; idx_t = ( (idx_t-1) + len_gps_data ) % len_gps_data ){
				num_cog_sample++;
				if(num_cog_sample==cog_sample_len){
					break;
				}
			}
			
			// �Ե�һ����Ч������Ϊ����ԭ��
			lon0 = gps_longitude[idx_t]; 
			lat0 = gps_latitude [idx_t];

			// �ӵ�һ������������Ч���ݿ�ʼ��������������ݵ������
			while(idx_t!=idx_tail){
				X[idx_XY] = sign(gps_longitude[idx_t]-lon0)*calSphereDist(lat0,lon0,lat0,gps_longitude[idx_t]); // ���Ƽ���X����					
				Y[idx_XY] = sign(gps_latitude[idx_t]-lat0)*calSphereDist(lat0,lon0,gps_latitude[idx_t],lon0);   // ���Ƽ���Y����
				idx_XY 		= (idx_XY+1)%len_gps_data;
				idx_t 		= (idx_t+1)%len_gps_data;
			}
			X[idx_XY] = sign(gps_longitude[idx_t]-lon0)*calSphereDist(lat0,lon0,lat0,gps_longitude[idx_t]);
			Y[idx_XY] = sign(gps_latitude[idx_t]-lat0)*calSphereDist(lat0,lon0,gps_latitude[idx_t],lon0);
			len_XY 		= idx_XY + 1;
			
			if(len_XY>=3){ // ������Ҫ3�����ݲ��ܹ���
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
					// �������ݹ��Ƴ���ֵ��ֵ��ȫ�ֱ�����������
					TP_theta  = theta;
					TP_theta_ = theta_;
					
					// ���ݺ��������ƫ�������������ľ�γ��
					CalNetLocation(TP_theta, TP_theta_);
					
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
		}else if(!offset_len_changed){
			// ����µ����ݵ���Ч����offset_len��û�б��޸ģ�������һ�γɹ����Ƴ������ݵ�
			jingdu = jingdu_tmp;
			weidu  = weidu_tmp;
			direction = direction_tmp;
		}else{
			// ����µ����ݵ���Ч����offset_len���޸��ˣ�����Ҫ���¼��������ľ�γ��
			CalNetLocation(TP_theta, TP_theta_);
			
			// ��¼���һ�ι��Ƴ��ľ�γ��
			jingdu_tmp = jingdu;
			weidu_tmp  = weidu;
			direction_tmp = direction;
		}

	}
	
	// ���������double�;�γ��ת��ΪAIS��Ϣ18��Ҫ�����ͣ����洢��flash�С����Ϊ��γ�����������򽫾�γ�ȣ���������Ϊ���루28λ��
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




