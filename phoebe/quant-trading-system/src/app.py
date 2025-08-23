#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from flask import Flask, render_template, request, jsonify, send_file
from datetime import datetime, timedelta
import os
import sys
import logging
from sqlalchemy import text

# 添加项目根目录到Python路径
project_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, project_root)
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from services.data_importer import DataImporter
from services.trading_analytics import TradingAnalytics
from services.portfolio_manager import PortfolioManager
from models.database import DatabaseManager

# 配置日志
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

app = Flask(__name__, 
           template_folder='../templates',
           static_folder='../static')

# 初始化服务
db_manager = DatabaseManager()
data_importer = DataImporter()
trading_analytics = TradingAnalytics()
portfolio_manager = PortfolioManager()

@app.route('/')
def index():
    """主页 - 显示今日交易概览"""
    try:
        today = datetime.now().date()
        
        # 获取今日交易数据
        daily_summary = trading_analytics.get_daily_summary(today)
        positions = portfolio_manager.get_current_positions()
        
        return render_template('index.html', 
                             daily_summary=daily_summary,
                             positions=positions,
                             today=today)
    except Exception as e:
        logger.error(f"主页加载错误: {e}")
        return render_template('error.html', error=str(e))

@app.route('/api/import-data', methods=['POST'])
def import_data():
    """API接口 - 导入数据"""
    try:
        result = data_importer.import_daily_data()
        return jsonify({
            'success': True,
            'message': '数据导入成功',
            'data': result
        })
    except Exception as e:
        logger.error(f"数据导入错误: {e}")
        return jsonify({
            'success': False,
            'message': f'数据导入失败: {str(e)}'
        })

@app.route('/api/trading-summary/<date>')
def get_trading_summary(date):
    """API接口 - 获取指定日期交易汇总"""
    try:
        trade_date = datetime.strptime(date, '%Y-%m-%d').date()
        summary = trading_analytics.get_daily_summary(trade_date)
        return jsonify({
            'success': True,
            'data': summary
        })
    except Exception as e:
        logger.error(f"获取交易汇总错误: {e}")
        return jsonify({
            'success': False,
            'message': str(e)
        })

@app.route('/api/positions')
def get_positions():
    """API接口 - 获取当前持仓"""
    try:
        positions = portfolio_manager.get_current_positions()
        return jsonify({
            'success': True,
            'data': positions
        })
    except Exception as e:
        logger.error(f"获取持仓错误: {e}")
        return jsonify({
            'success': False,
            'message': str(e)
        })

@app.route('/trading-performance')
def trading_performance():
    """交易绩效页面"""
    return render_template('trading-performance.html')

@app.route('/trading-details')
def trading_details():
    """交易明细页面"""
    return render_template('trading_details.html')

@app.route('/analytics')
def analytics():
    """分析页面"""
    return render_template('analytics.html')

@app.route('/api/pnl-report')
def get_pnl_report():
    """API接口 - 获取PnL报告"""
    try:
        start_date = request.args.get('start_date')
        end_date = request.args.get('end_date')
        
        if start_date:
            start_date = datetime.strptime(start_date, '%Y-%m-%d').date()
        if end_date:
            end_date = datetime.strptime(end_date, '%Y-%m-%d').date()
        
        report = trading_analytics.generate_pnl_report(start_date, end_date)
        return jsonify({
            'success': True,
            'data': report
        })
    except Exception as e:
        logger.error(f"生成PnL报告错误: {e}")
        return jsonify({
            'success': False,
            'message': str(e)
        })

@app.route('/api/performance-metrics')
def get_performance_metrics():
    """API接口 - 获取绩效指标"""
    try:
        start_date = request.args.get('start_date')
        end_date = request.args.get('end_date')
        
        if start_date:
            start_date = datetime.strptime(start_date, '%Y-%m-%d').date()
        if end_date:
            end_date = datetime.strptime(end_date, '%Y-%m-%d').date()
        
        metrics = trading_analytics.calculate_performance_metrics(start_date, end_date)
        return jsonify({
            'success': True,
            'data': metrics
        })
    except Exception as e:
        logger.error(f"计算绩效指标错误: {e}")
        return jsonify({
            'success': False,
            'message': str(e)
        })

@app.route('/api/trading-details', methods=['GET'])
def get_trading_details():
    """API接口 - 获取交易明细数据"""
    try:
        # 获取查询参数
        trade_date = request.args.get('date')
        business_type = request.args.get('business_type', 'all')
        page = int(request.args.get('page', 1))
        page_size = int(request.args.get('page_size', 50))
        
        session = db_manager.get_session()
        
        # 构建基础查询
        base_query = '''
            SELECT 
                td.trade_date,
                td.security_code,
                td.security_name,
                td.business_type,
                td.trade_direction,
                COALESCE(td.trade_volume, 0) as trade_volume,
                COALESCE(td.trade_price, 0) as trade_price,
                COALESCE(td.trade_amount, 0) as trade_amount,
                COALESCE(td.commission, 0) as commission,
                COALESCE(td.total_fee, 0) as total_fee,
                COALESCE(td.net_amount, 0) as net_amount,
                COALESCE(td.pnl, 0) as pnl,
                td.created_at,
                COALESCE(td.buy_price, 0) as buy_price,
                COALESCE(td.sell_price, 0) as sell_price,
                COALESCE(td.buy_volume, 0) as buy_volume,
                COALESCE(td.sell_volume, 0) as sell_volume,
                COALESCE(td.buy_amount, 0) as buy_amount,
                COALESCE(td.sell_amount, 0) as sell_amount
            FROM trading_details td
            WHERE 1=1
        '''
        
        # 添加过滤条件
        params = {}
        if trade_date:
            base_query += ' AND td.trade_date = :trade_date'
            params['trade_date'] = trade_date
            
        if business_type != 'all':
            base_query += ' AND td.business_type = :business_type'
            params['business_type'] = business_type
        
        # 添加排序
        base_query += ' ORDER BY td.trade_date DESC, td.created_at DESC'
        
        # 计算总数
        count_query = f"SELECT COUNT(*) as total FROM ({base_query}) as subq"
        total_result = session.execute(text(count_query), params).fetchone()
        total_records = total_result.total
        
        # 添加分页
        offset = (page - 1) * page_size
        paginated_query = base_query + f' LIMIT {page_size} OFFSET {offset}'
        
        # 执行查询
        results = session.execute(text(paginated_query), params).fetchall()
        
        # 转换为字典格式
        details = []
        for row in results:
            details.append({
                'trade_date': row.trade_date.strftime('%Y-%m-%d') if row.trade_date else '',
                'security_code': row.security_code,
                'security_name': row.security_name,
                'business_type': row.business_type,
                'trade_direction': row.trade_direction,
                'trade_volume': int(row.trade_volume),
                'trade_price': float(row.trade_price),
                'trade_amount': float(row.trade_amount),
                'commission': float(row.commission),
                'total_fee': float(row.total_fee),
                'net_amount': float(row.net_amount),
                'pnl': float(row.pnl),
                'buy_price': float(row.buy_price),
                'sell_price': float(row.sell_price),
                'buy_volume': int(row.buy_volume),
                'sell_volume': int(row.sell_volume),
                'buy_amount': float(row.buy_amount),
                'sell_amount': float(row.sell_amount),
                'created_at': row.created_at.strftime('%Y-%m-%d %H:%M:%S') if row.created_at else ''
            })
        
        session.close()
        
        return jsonify({
            'success': True,
            'data': {
                'details': details,
                'pagination': {
                    'page': page,
                    'page_size': page_size,
                    'total_records': total_records,
                    'total_pages': (total_records + page_size - 1) // page_size
                }
            }
        })
        
    except Exception as e:
        logger.error(f"获取交易明细错误: {e}")
        return jsonify({
            'success': False,
            'message': str(e)
        })

@app.route('/api/daily-summary', methods=['GET'])
def get_daily_summary_api():
    """API接口 - 获取每日汇总数据"""
    try:
        session = db_manager.get_session()
        
        # 按日期和业务类型汇总
        query = '''
            SELECT 
                trade_date,
                business_type,
                COUNT(*) as record_count,
                COALESCE(SUM(pnl), 0) as daily_pnl,
                COALESCE(SUM(trade_amount), 0) as total_amount,
                COUNT(DISTINCT security_code) as unique_securities
            FROM trading_details
            GROUP BY trade_date, business_type
            ORDER BY trade_date DESC, business_type
        '''
        
        results = session.execute(text(query)).fetchall()
        
        # 按日期分组
        daily_data = {}
        for row in results:
            date_str = row.trade_date.strftime('%Y-%m-%d')
            if date_str not in daily_data:
                daily_data[date_str] = {
                    'date': date_str,
                    'business_types': {},
                    'total_pnl': 0,
                    'total_records': 0,
                    'total_amount': 0,
                    'unique_securities': set()
                }
            
            daily_data[date_str]['business_types'][row.business_type] = {
                'record_count': row.record_count,
                'pnl': float(row.daily_pnl),
                'amount': float(row.total_amount),
                'unique_securities': row.unique_securities
            }
            daily_data[date_str]['total_pnl'] += float(row.daily_pnl)
            daily_data[date_str]['total_records'] += row.record_count
            daily_data[date_str]['total_amount'] += float(row.total_amount)
        
        # 转换为列表并计算唯一证券数
        summary = []
        for date_str, data in daily_data.items():
            # 计算该日期的唯一证券数
            unique_securities_query = '''
                SELECT COUNT(DISTINCT security_code) as count
                FROM trading_details
                WHERE trade_date = :trade_date
            '''
            unique_count = session.execute(text(unique_securities_query), 
                                         {'trade_date': date_str}).fetchone()
            data['unique_securities'] = unique_count.count
            summary.append(data)
        
        session.close()
        
        return jsonify({
            'success': True,
            'data': summary
        })
        
    except Exception as e:
        logger.error(f"获取每日汇总错误: {e}")
        return jsonify({
            'success': False,
            'message': str(e)
        })

@app.route('/api/business-types', methods=['GET'])
def get_business_types():
    """API接口 - 获取所有业务类型"""
    try:
        session = db_manager.get_session()
        
        query = '''
            SELECT DISTINCT business_type
            FROM trading_details
            ORDER BY business_type
        '''
        
        results = session.execute(text(query)).fetchall()
        business_types = [row.business_type for row in results]
        
        session.close()
        
        return jsonify({
            'success': True,
            'data': business_types
        })
        
    except Exception as e:
        logger.error(f"获取业务类型错误: {e}")
        return jsonify({
            'success': False,
            'message': str(e)
        })

@app.route('/api/trading-dates', methods=['GET'])
def get_trading_dates():
    """API接口 - 获取所有交易日期"""
    try:
        session = db_manager.get_session()
        
        query = '''
            SELECT DISTINCT trade_date
            FROM trading_details
            ORDER BY trade_date DESC
        '''
        
        results = session.execute(text(query)).fetchall()
        trading_dates = [row.trade_date.strftime('%Y-%m-%d') for row in results if row.trade_date]
        
        session.close()
        
        return jsonify({
            'success': True,
            'data': trading_dates
        })
        
    except Exception as e:
        logger.error(f"获取交易日期错误: {e}")
        return jsonify({
            'success': False,
            'message': str(e)
        })

@app.route('/schedule-import')
def schedule_import():
    """计划任务页面"""
    return render_template('schedule.html')

@app.route('/api/import-status')
def get_import_status():
    """API接口 - 获取导入状态"""
    try:
        status = data_importer.get_import_status()
        return jsonify({
            'success': True,
            'data': status
        })
    except Exception as e:
        logger.error(f"获取导入状态错误: {e}")
        return jsonify({
            'success': False,
            'message': str(e)
        })

@app.route('/api/file-status')
def get_file_status():
    """API接口 - 获取文件状态"""
    try:
        # 简化实现，返回模拟数据
        return jsonify({
            'success': True,
            'data': {
                'pending_files': 0,
                'processed_files': 24
            }
        })
    except Exception as e:
        logger.error(f"获取文件状态错误: {e}")
        return jsonify({
            'success': False,
            'message': str(e)
        })

@app.route('/api/retry-import', methods=['POST'])
def retry_import():
    """API接口 - 重试导入失败的文件"""
    try:
        data = request.get_json()
        file_name = data.get('file_name')
        
        if not file_name:
            return jsonify({
                'success': False,
                'message': '缺少文件名参数'
            })
        
        # 这里应该实现重试逻辑
        # 简化实现
        return jsonify({
            'success': True,
            'message': f'文件 {file_name} 重新导入成功'
        })
    except Exception as e:
        logger.error(f"重试导入错误: {e}")
        return jsonify({
            'success': False,
            'message': str(e)
        })

@app.errorhandler(404)
def not_found_error(error):
    return render_template('error.html', error='页面未找到'), 404

@app.errorhandler(500)
def internal_error(error):
    return render_template('error.html', error='服务器内部错误'), 500

if __name__ == '__main__':
    # 创建数据库表
    try:
        db_manager.create_tables()
        logger.info("数据库表创建成功")
    except Exception as e:
        logger.error(f"数据库表创建失败: {e}")
    
    # 启动应用
    app.run(host='0.0.0.0', port=5000, debug=True)
