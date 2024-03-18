try:
    from setuptools import setup, find_packages
except ImportError:
    from distutils.core import setup, find_packages

setup(
    name='pymoneta',  #
    author='bo.wang',  #
    version='0.3.3',  #
    license='MIT',

    description='moneta',  #
    long_description='''long description''',
    author_email='wang.b@quantstage.com',  #
    url='https://www.quantstage.com',  #
    
    packages= find_packages(exclude=['pymoneta',]),
    # packages=["pyolla"],
    # keywords='NLP,tokenizing,Chinese word segementation',
    package_data={'':['*.*']},

    install_requires=[
        "redis >= 1.0.0",
        "pandas >= 1.0.0",
        "pyarrow >= 3.0.0",
        "requests >= 2.0.0",
        "lxml >= 5.0.0",
    ],
    python_requires='>=3',
    classifiers=[
        # 'Development Status :: 4 - Beta',
        # 'Operating System :: Microsoft'  #
        'Intended Audience :: Developers',
        # 'License :: OSI Approved :: MIT License',
        # 'License :: OSI Approved :: BSD License',
        'Programming Language :: Python',
        'Programming Language :: Python :: 3',
        'Topic :: Software Development :: Libraries'
    ],
    zip_safe = False,
)