from flask import Blueprint, request, jsonify
from sqlalchemy import text
from datetime import datetime, date
from config.database import DatabaseManager

trading_details_bp = Blueprint('trading_details', __name__)
db_manager = DatabaseManager()

@trading_details_bp.route('/api/trading-details', methods=['GET'])
def get_trading_details():
    """获取交易明细数据"""
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
                td.trade_volume,
                td.trade_price,
                td.trade_amount,
                td.commission,
                td.total_fee,
                td.net_amount,
                td.pnl,
                td.created_at
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
                'trade_date': row.trade_date.strftime('%Y-%m-%d'),
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
                'created_at': row.created_at.strftime('%Y-%m-%d %H:%M:%S')
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
        return jsonify({
            'success': False,
            'error': str(e)
        }), 500

@trading_details_bp.route('/api/daily-summary', methods=['GET'])
def get_daily_summary():
    """获取每日汇总数据"""
    try:
        session = db_manager.get_session()
        
        # 按日期和业务类型汇总
        query = '''
            SELECT 
                trade_date,
                business_type,
                COUNT(*) as record_count,
                SUM(pnl) as daily_pnl,
                SUM(trade_amount) as total_amount,
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
        return jsonify({
            'success': False,
            'error': str(e)
        }), 500

@trading_details_bp.route('/api/business-types', methods=['GET'])
def get_business_types():
    """获取所有业务类型"""
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
        return jsonify({
            'success': False,
            'error': str(e)
        }), 500

@trading_details_bp.route('/api/trading-dates', methods=['GET'])
def get_trading_dates():
    """获取所有交易日期"""
    try:
        session = db_manager.get_session()
        
        query = '''
            SELECT DISTINCT trade_date
            FROM trading_details
            ORDER BY trade_date DESC
        '''
        
        results = session.execute(text(query)).fetchall()
        dates = [row.trade_date.strftime('%Y-%m-%d') for row in results]
        
        session.close()
        
        return jsonify({
            'success': True,
            'data': dates
        })
        
    except Exception as e:
        return jsonify({
            'success': False,
            'error': str(e)
        }), 500
