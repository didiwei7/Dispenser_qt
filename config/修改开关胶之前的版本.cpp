// Thread 点胶1
void Workflow::thread_glue_1()
{
	if (!(init_card() == 1)) return;

	// 从CCD获取到的偏移 X, Y, A
	float ccd_offset_x, ccd_offset_y, ccd_offset_A;

	// 镭射获取的高度
	float laser_num = 0;
	float laser_z[4];
	float laser_z_average = 0;

	// 4段圆弧的圆心坐标
	float center_result_x[4];
	float center_result_y[4];

	MatrixXf work_matrix;					// CCD点位旋转平移后的矩阵
	QVector<CCDGlue>::iterator iter_cmd;	// Vector的头尾
	CCDGlue current_cmd;					// 当前点位
	int     current_cmd_num;				// 当前点位索引
	
	int step_glue1 = 0;

	while (close_thread_glue_1 == false)
	{
		switch (step_glue1)
		{
		case 0:		// 等待触发
		{
			if (start_thread_glue_1 == false)
			{
				Sleep(2);
				step_glue1 = 0;
			}
			else
			{
				step_glue1 = 10;
			}
		}
		break;

		case 10:	// 检查是否配置了该流程
		{
			if (is_config_gluel == false)
			{
				step_glue1 = 7777;
			}
			else
			{
				step_glue1 = 20;
			}
		}
		break;

		case 20:	// 设置速度, 模式
		{
			// 设置所有轴速度, 模式
			// set_speed_mode()
			step_glue1 = 30;
		}
		break;

		case 30:	// Z轴到安全位
		{
			move_axis_abs(AXISNUM::Z, 0);
			step_glue1 = 40;
		}
		break;

		case 40:	// 提前切换图像程序
		{
			// 发消息
			step_glue1 = 50;
		}
		break;

		case 50:	// 点前清胶
		{
			// 【1】 到清胶安全点
			move_point_name("xx");
			wait_allaxis_stop();

			// 【2】 判断清胶气缸
			if (true)
			{
				QMessageBox::warning(NULL, QStringLiteral("警告"), QStringLiteral("清胶气缸状态错误, 请检查"));
				step_glue1 = 8888;
			}

			// 【4】 到清胶点
			move_point_name("xx");
			wait_allaxis_stop();

			// 【5】 清胶气缸夹紧


			// 【6】 到清胶安全点, 此点位安全高度为 0
			move_point_name("xx");
			wait_allaxis_stop();

			// 【7】 清胶气缸松开

			step_glue1 = 60;
		}
		break;

		case 60:	// 到 "点胶1" 拍照点
		{
			// 到点胶1拍照点
			move_point_name("xx");
			wait_allaxis_stop();

			step_glue1 = 70;
		}
		break;

		case 70:	// 拍照
		{
			// 发消息拍照

			step_glue1 = 80;
		}
		break;

		case 80:	// 等待获取偏移 ccd_offset_x, ccd_offset_y, ccd_offset_z
		{
			if (receivedMsg_ccd == "" || receivedMsg_ccd.length() < 5)
			{
				step_glue1 = 80;
			}
			else
			{
				if (receivedMsg_ccd.split("]").last() == "-1")
				{
					step_glue1 = 7777;
				}
				else
				{
					ccd_offset_x = receivedMsg_ccd.split("]").at(1).toFloat();
					ccd_offset_y = receivedMsg_ccd.split("]").at(2).toFloat();
					ccd_offset_A = receivedMsg_ccd.split("]").at(3).toFloat();

					receivedMsg_ccd = "";

					step_glue1 = 90;
				}
			}
		}
		break;

		case 90:	// 计算矩阵偏移
		{
			work_matrix = CalCCDGluePoint(vec_ccdGlue_1, ccd_offset_x, ccd_offset_y, ccd_offset_A, org_ccdglue_x[CCDORG::GLUE1ORG], org_ccdglue_y[CCDORG::GLUE1ORG]);
		}
		break;

		case 100:	// 初始化所有标志位, 开启速度前瞻, 设置插补速度
		{
			iter_cmd = vec_ccdGlue_1.begin();
			current_cmd_num = 0;
			
			adt8949_set_speed_pretreat_mode(0, 1);
			set_inp_speed(1);

			step_glue1 = 110;

		}
		break;

		case 110:	// 判断 "结束" 和 "CCDGlue.type"
		{
			if ( iter_cmd == vec_ccdGlue_1.end())
			{
				step_glue1 = 500;	
			}
			else
			{
				if (vec_ccdGlue_1.at(current_cmd_num).type == QString("null"))
				{
					step_glue1 = 500;
				}
				else if (current_cmd.type == QString("line"))
				{
					current_cmd = vec_ccdGlue_1.at(current_cmd_num);
					step_glue1 = 200;
				}
				else if (current_cmd.type == QString("circle"))
				{
					current_cmd = vec_ccdGlue_1.at(current_cmd_num);
					step_glue1 = 400;
				}
				else
				{
					QMessageBox::warning(NULL, QStringLiteral("警告"), QStringLiteral("点位解析错误"));
					step_glue1 = 9999;
				}
			}
		}
		break;


		/********************** 直线插补 **********************/
		case 200:	// 直线插补
		{
			// 判断 "镭射"
			if (current_cmd.laser)
			{
				step_glue1 = 300;
			}
			else
			{
				step_glue1 = 210;
			}
		}
		break;

		case 210:	// 是否开胶
		{
			if (current_cmd.open)
			{
				if (current_cmd.openAdvance > 0 || current_cmd.openDelay <= 0)
				{
					// 提前开胶
				}
				else if (current_cmd.openAdvance <= 0 || current_cmd.openDelay > 0)
				{
					// 滞后开胶
				}
				else
				{
					// 立即开胶
					adt8949_set_fifo_io(0, 18, 1, -1);
				}
			}

			step_glue1 = 220;
		}
		break;

		case 220:	// 是否提前滞后关胶
		{
			if (current_cmd.closeAdvance > 0 || current_cmd.closeDelay <= 0)
			{
				// 提前关胶
			}
			else if (current_cmd.closeAdvance <= 0 || current_cmd.closeDelay > 0)
			{
				// 滞后关胶
			}
			step_glue1 = 230;
		}
		break;

		case 230:	// 直线插补
		{
			float move_pos[3];
			move_pos[0] = work_matrix(current_cmd_num, 0) + distance_ccd_needle_x + calib_offset_x;
			move_pos[1] = work_matrix(current_cmd_num, 1) + distance_ccd_neddle_y + calib_offset_y;
			move_pos[2] = work_matrix(current_cmd_num, 2) + distance_ccd_needle_x + calib_offset_z; // +"镭射高度";

			// 直线插补
			move_inp_abs_line3(move_pos[0], move_pos[1], move_pos[2]);

			step_glue1 = 240;
		}
		break;

		case 240:	// 是否立即关胶
		{
			if (current_cmd.close)
			{
				// 关胶
				write_out_bit(0, 0);
			}

			step_glue1 = 250;
		}
		break;

		case 250:	// 重置标志位, 跳回
		{
			iter_cmd++;
			current_cmd_num++;
			step_glue1 = 110;
		}
		break;


		/********************** 镭射测量 **********************/
		case 300:	// 镭射测量
		{
			float move_pos[3];

			// move_pos[0] = work_matrix[current_cmd_num, 0] + ;
		}
		break;

		
		/********************** 圆弧插补 **********************/
		case 400:	// 圆弧插补
		{
			// 【1】 打开插补缓存区

			// 【2】 设置插补速度

			step_glue1 = 410;
		}
		break;

		case 410:	// 是否开胶
		{
			if (current_cmd.open)
			{
				if (current_cmd.openAdvance > 0 || current_cmd.openDelay <= 0)
				{
					// 提前开胶
				}
				else if (current_cmd.openAdvance <= 0 || current_cmd.openDelay > 0)
				{
					// 滞后开胶
				}
				else
				{
					// 立即开胶
					write_out_bit(0, 1);
				}
			}

			step_glue1 = 420;
		}
		break;

		case 420:	// 是否 提前, 滞后关胶
		{
			if (current_cmd.closeAdvance > 0 || current_cmd.closeDelay <= 0)
			{
				// 提前关胶
			}
			else if (current_cmd.closeAdvance <= 0 || current_cmd.closeDelay > 0)
			{
				// 滞后关胶
			}
			step_glue1 = 430;
		}
		break;

		case 430:	// 圆弧插补
		{
			float move_pos[3];
			move_pos[0] = work_matrix(current_cmd_num, 0) + distance_ccd_needle_x + calib_offset_x;
			move_pos[1] = work_matrix(current_cmd_num, 1) + distance_ccd_neddle_y + calib_offset_y;
			move_pos[2] = work_matrix(current_cmd_num, 2) + distance_ccd_needle_x + calib_offset_z; // +"镭射高度";

			// 开始插补
			move_inp_abs_line3(move_pos[0], move_pos[1], move_pos[2]);
			wait_inp_finish();
			step_glue1 = 440;
		}
		break;

		case 440:	// 是否立即关胶
		{
			if (current_cmd.close)
			{
				// 关胶
				write_out_bit(0, 0);
			}

			step_glue1 = 450;
		}
		break;

		case 450:	// 重置标志位, 跳回
		{
			iter_cmd++;//映射计数//
			current_cmd_num++;//指令计数//
			step_glue1 = 110;
		}
		break;


		/********************** 点位解析完毕后做的事 **********************/
		case 500:	// 关闭插补缓存区
		{
			step_glue1 = 510;
		}
		break;

		case 510:	// 设置运动速度, Z轴到安全位
		{
			// 【1】 等待插补完成
			wait_inp_finish();

			// 【2】 关闭插补
			move_axis_abs(AXISNUM::Z, 0);
			wait_axis_stop(AXISNUM::Z);
			step_glue1 = 520;
		}
		break;

		case 520:	// 重置计数
		{
			iter_cmd = 0;
			current_cmd_num = 0;

			step_glue1 = 8888;
		}



		case 6666:	// 流程正常执行完毕, 跳回0, 等待下次触发流程
		{
			// 【1】 刷新流程标志位
			iter_cmd = vec_ccdGlue_1.begin();
			current_cmd_num = 0;
			laser_num = 0;

			// 【2】 发消息
			emit changedRundataText(QStringLiteral("点胶1已完成"));
			writRunningLog(QStringLiteral("点胶1已完成"));

			start_thread_glue_1 = false;
			step_glue1 = 0;
		}
		break;

		case 7777:	// 流程非正常执行完毕, 跳回0, 等待下次触发流程
		{
			// 【1】 Z轴到安全位
			move_axis_abs(AXISNUM::Z, 0);
			wait_axis_stop(AXISNUM::Z);

			// 【2】 刷新流程标志位
			iter_cmd = vec_ccdGlue_1.begin();
			current_cmd_num = 0;
			laser_num = 0;

			// 【3】 发消息
			emit changedRundataText(QStringLiteral("点胶1执行失败, 正在跳过"));
			writRunningLog(QStringLiteral("点胶1执行失败, 正在跳过"));

			start_thread_glue_1 = false;
			step_glue1 = 0;
		}
		break;

		case 8888:	// 线程正常执行完毕, 关闭该线程
		{
			emit changedRundataText(QStringLiteral("点胶1线程正常结束"));
			writRunningLog(QStringLiteral("点胶1线程正常结束"));

			close_thread_glue_1 = true;
			step_glue1 = 0;
		}
		break;

		case 9999:	// 线程非正常执行完毕, 关闭该线程
		{
			// 安全退出该线程
			close_thread_glue_1 = true;
			step_glue1 = 0;

			// 触发停止信号
			stop_allaxis();
		}
		break;

		default:
			break;
		}
	}

}